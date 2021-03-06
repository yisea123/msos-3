// This file is part of MSOS project.
// Copyright (C) 2020 Mateusz Stadnik
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

#include "msos/drivers/i_driver.hpp"

#include <hal/gpio/digital_input_output_pin.hpp>

namespace msos
{
namespace drivers
{
namespace character
{

class GpioDriver : public IDriver
{
public:
    void load() override;
    void unload() override;

    std::unique_ptr<fs::IFile> file(std::string_view path, int flags) override;
};

} // namespace character
} // namespace drivers
} // namespace msos
