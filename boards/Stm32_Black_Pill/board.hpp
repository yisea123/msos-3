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

#pragma once

// #include <devices/arm/stm32/stm32f1/stm32f103c8t6/gpio.hpp>
// #include <devices/arm/stm32/stm32f1/stm32f103c8t6/usart.hpp>
// #include <devices/arm/stm32/stm32f1/stm32f103c8t6/i2c.hpp>
#include <devices/arm/stm32/stm32f1/stm32f103c8t6/usart.hpp>
#include <hal/interfaces/usart.hpp>

#include <devices/arm/stm32/stm32f1/stm32f103c8t6/usart.hpp>
#include <devices/arm/stm32/stm32f1/stm32f103c8t6/i2c.hpp>

namespace board
{

void board_init();

namespace gpio
{

// extern hal::gpio::DigitalOutputPin LED_GREEN; = hal::devices::gpio::PB1::OutputType;
// using LED_RED = hal::devices::gpio::PB0::OutputType;
// using LED_YELLOW = hal::devices::gpio::PA7::OutputType;
// using LED_BLUE = hal::devices::gpio::PA3::OutputType;

// using LEFT_KEY = hal::devices::gpio::PA12::InputType;
// using MID_KEY = hal::devices::gpio::PB14::InputType;
// using RIGHT_KEY = hal::devices::gpio::PA15::InputType;

}

namespace interfaces
{

std::array<hal::interfaces::Usart*, 1>& usarts();
hal::interfaces::I2C& LCD_I2C();

}

}
