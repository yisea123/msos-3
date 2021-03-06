# This file is part of MSOS project. This is simple OS for embedded development devices.
# Copyright (C) 2020 Mateusz Stadnik
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

add_executable(msos_ls)

target_sources(msos_ls
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)


# TODO: smarter way to inherit flags
target_link_libraries(msos_ls
    PRIVATE
        module_flags
)

add_module(msos_ls msos_ls)

if (${arch} STREQUAL "ARM")
    set (suffix ".bin")
    set (prefix "")
else ()
    set (suffix "")
    set (prefix "")
endif ()

add_custom_command(
    TARGET msos_ls
    POST_BUILD
    COMMAND mkdir -p ${PROJECT_BINARY_DIR}/rootfs/bin
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${prefix}msos_ls${suffix} ${PROJECT_BINARY_DIR}/rootfs/bin/
)
