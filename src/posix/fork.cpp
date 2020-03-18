// This file is part of MSOS project. This is simple OS for embedded development devices.
// Copyright (C) 2019 Mateusz Stadnik
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "unistd.h"

#include <cstdint>
#include <cstdio>

#include "msos/kernel/process/context_switch.hpp"
#include "msos/kernel/process/process_manager.hpp"
#include "msos/kernel/process/scheduler.hpp"
#include "msos/kernel/process/process.hpp"
#include "msos/kernel/process/registers.hpp"
#include "msos/kernel/synchronization/mutex.hpp"

#include <hal/time/time.hpp>
#include <hal/core/criticalSection.hpp>

#include <stm32f10x.h>

extern "C"
{

    pid_t _fork(void);
    pid_t getpid();
    void dump_registers(void);

    pid_t process_fork(uint32_t sp, uint32_t return_address, msos::kernel::process::RegistersDump* registers);

    int was_current_process_deleted();
    void update_stack_pointer(const std::size_t* stack);
    void set_first_task_processed();

    const std::size_t* get_next_task();


    void SysTick_Handler(void);
    void PendSV_Handler(void);

    void store_context();
    void load_context(uint32_t);
}

static inline uint32_t get_PRIMASK()
{
    uint32_t mask;
    asm inline volatile(
        "mrs %0, primask\n" : "=r"(mask));
    return mask;
}

extern std::size_t __stack_start;
std::size_t* stack_start = &__stack_start;
constexpr std::size_t default_stack_size = 1024;

msos::kernel::process::ProcessManager* processes;

namespace msos
{
namespace kernel
{
namespace process
{

Scheduler* scheduler;
}
}
}



static bool was_initialized = false;
static bool first = true;

int was_current_process_deleted()
{
    return msos::kernel::process::scheduler->current_process_was_deleted();
}

void update_stack_pointer(const std::size_t* stack)
{
    msos::kernel::process::scheduler->current_process().current_stack_pointer(stack);
}

void set_first_task_processed()
{
    first = false;
}

void __attribute__((naked)) store_context()
{
    asm volatile inline (
        "push {r0}\n"

        "pop {r0}\n"
    );
}

void __attribute__((naked)) load_context(uint32_t sp)
{
    asm volatile inline (
        "ldmia r0!, {r4 - r11, lr}\n"
        "msr psp, r0\n"
    );
}

void __attribute__((naked)) PendSV_Handler(void)
{
    // asm volatile inline("mov r0, %0" : : "r"(first))
    // asm volatile inline(
    //     "cmp r0, #1\n"
    //     "bne process_first\n"
    //     "bl was_current_process_deleted\n"
    //     "cmp r0, #1"
    //     "beq "
    //     "process_first:\n\t"
    //     ""
    // )
    // if (!first && !msos::kernel::process::scheduler->current_process_was_deleted())
    // {
    //     std::size_t* stack;
    //     asm volatile inline (
    //         "mov %0, r0" : "=r" (stack)
    //     );
    //     msos::kernel::process::scheduler->current_process().current_stack_pointer(stack);
    // }
    // else
    // {
    //     first = false;
    // }

    // asm volatile inline (
    //         "mov r0, %0\n" : : "r" (msos::kernel::process::scheduler->schedule_next())
    // );

    // asm volatile inline (
    //         "ldmia r0!, {r4 - r11, lr}\n"
    //         "msr psp, r0\n"
    //         );
    asm volatile inline (
        "mrs r0, psp \n"
        "stmdb r0!, {r4 - r11, lr}\n"
        "bl update_stack_pointer\n"
        "bl get_next_task\n"
        "ldmia r0!, {r4 - r11, lr}\n"
        "msr psp, r0\n"
        "bx lr\n"
    );
}

const std::size_t* get_next_task()
{
    return msos::kernel::process::scheduler->schedule_next();
}

pid_t getpid()
{
    return msos::kernel::process::scheduler->current_process().pid();
}

void pend()
{
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

extern "C"
{
    volatile int is_pendsv_blocked = false;
}

pid_t root_process(const std::size_t process)
{
    if (processes != nullptr)
    {
        delete processes;
    }
    if (msos::kernel::process::scheduler != nullptr)
    {
        delete msos::kernel::process::scheduler;
    }

    processes = new msos::kernel::process::ProcessManager();
    msos::kernel::process::scheduler = new msos::kernel::process::Scheduler(*processes);

    auto& root_process = processes->create_process(process, default_stack_size);
    processes->print();
    msos::kernel::process::scheduler->schedule_next();
    NVIC_SetPriority(PendSV_IRQn, 0xff); /* Lowest possible priority */
    NVIC_SetPriority(SVCall_IRQn, 0x01); /* Lowest possible priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00); /* Highest possible priority */

    hal::time::Time::add_handler([](std::chrono::milliseconds time)
    {
        static std::chrono::milliseconds last_time = time;

        if (std::chrono::duration_cast<std::chrono::milliseconds>(time - last_time) >= std::chrono::milliseconds(100))
        {
            if (get_PRIMASK() != 1)
            {
                if (!is_pendsv_blocked)
                {
                    pend();
                }
            }
            last_time = time;
        }

    });

    asm volatile inline(
        "mov r0, #9\n"
        "svc 0\n");

    while(true)
    {
    }
}


pid_t process_fork(uint32_t sp, uint32_t return_address, msos::kernel::process::RegistersDump* registers)
{
    auto& parent_process = msos::kernel::process::scheduler->current_process();
    /* sizeof(size_t), LR is pushed to stack in _fork function */
    std::size_t diff = (reinterpret_cast<std::size_t>(parent_process.stack_pointer()) + parent_process.stack_size()) - sp - sizeof(size_t);
    auto& child_process = processes->create_process(parent_process, diff, return_address, registers);

    processes->print();
    return child_process.pid();
}


extern "C"
{
pid_t _fork_p(uint32_t sp, uint32_t link_register);

volatile msos::kernel::process::RegistersDump registers;
volatile uint32_t syscall_return_code = 0;

}
// pid_t __attribute__((naked)) _fork_p(uint32_t sp, uint32_t link_register)
// {
//     asm volatile inline("push {r1, r2, r3, r4, r5, lr}");

//     asm volatile inline("mov r5, %0" : : "r"(&registers));
//     asm volatile inline("mov r4, %0" : : "r"(sp));
//     asm volatile inline("mov r1, %0" : : "r"(link_register));
//     asm volatile inline("mov r2, %0" : : "r"(&syscall_return_code));
//     asm volatile inline("mov r0, #3");
//     asm volatile inline(//"isb   \n\t"
//                         //"dsb   \n\t"
//                         "svc 0 \n\t"
//                         "wfi   \n\t"
//                         "isb   \n\t"
//                         "dsb   \n\t"
//                         );
//     asm volatile inline("mov r0, %0" : "=r"(syscall_return_code));
//     asm volatile inline("pop {r1, r2, r3, r4, r5, pc}");
// }

// void lock_pendsv()
// {
//     is_pendsv_blocked = true;
// }

// void unlock_pendsv()
// {
//     is_pendsv_blocked = false;

// }

// // This function must ensure that stack is not touched inside, but may calls such functions
// pid_t __attribute__((naked)) _fork(void)
// {
//     // asm volatile inline("ldr r0, =is_pendsv_blocked");
//     asm volatile inline("push {r1}\n\t"
//                         "mov r1, #1\n\t"
//                         "str r1, [r0]\n\t"
//                         "pop {r1}\n\t");
//     asm volatile inline ("mov r0, %0" : : "i"(reinterpret_cast<uint32_t>(&registers)));
//     asm volatile inline ("stmia r0, {r1 - r12, lr}");
//     asm volatile inline("isb\n\t"
//                         "push {lr}\n\t");

//     asm volatile inline("mov r1, lr\n\t"
//                         "mrs r0, PSP\n\t"
//                         "isb\n\t"
//                         "bl _fork_p\n\t");

//     asm volatile inline("mov r0, %0" : : "i"(reinterpret_cast<uint32_t>(&is_pendsv_blocked)));
//     asm volatile inline("push {r1}\n\t"
//                         "mov r1, #0\n\t"
//                         "str r1, [r0]\n\t"
//                         "pop {r1}\n\t");

//     asm volatile inline("pop {pc}\n\t");
// }

