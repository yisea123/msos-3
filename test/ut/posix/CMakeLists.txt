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

add_executable (posix_tests)

target_sources(posix_tests
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/dirent_utils_tests.cpp
)

target_link_libraries(posix_tests
    PRIVATE
        gtest
        gtest_main
        gmock
        msos_posix
        # eul_filesystem
        # msos_fs
)

target_compile_options(posix_tests
    PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++2a -Werror -Wall -Wextra -Wpedantic -Wconversion -Wcast-align -Wunused -Wshadow -Wold-style-cast -Wpointer-arith -Wcast-qual -Wno-missing-braces>
        $<$<CONFIG:DEBUG>:-Og -g>
        $<$<CONFIG:RELEASE>:-Os>
)

target_include_directories(posix_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}
)

add_test(posix posix_tests
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})


