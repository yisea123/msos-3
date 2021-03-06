// This file is part of MSOS project.
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

#pragma once


#include "msos/kernel/process/process.hpp"

namespace msos
{
namespace kernel
{
namespace process
{

class IScheduler
{
public:
    virtual const Process* current_process() const = 0;
    virtual Process* current_process() = 0;

    virtual void delete_process(pid_t pid) = 0;
    virtual void schedule_next() = 0;
    virtual void unblock_all(void* semaphore) = 0;
};

class Scheduler
{
public:
    static IScheduler* get();
    static void set(IScheduler* scheduler);
};


} // namespace process
} // namespace kernel
} // namespace msos

