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

#include <stdint.h>

/* IO call serializer */

//  one cell is one bit
// +---+---+---+---+
// |    IO call    |
// |    number     |
// +---+---+---+---+
// |     type      |
// |               |
// +---+---+---+---+
// |               |
// |     size      |
// |               |
// |   +---+---+---|
// |   | direction |
// +---+---+---+---+

#define _IOC_DIRECTION_NONE 0U
#define _IOC_DIRECTION_READ 2U
#define _IOC_DIRECTION_WRITE 1U

#define _IOC_NUMBER_SIZE 8
#define _IOC_TYPE_SIZE 8
#define _IOC_SIZE_SIZE 14
#define _IOC_DIRECTION_SIZE 2

#define _IOC_NUMBER_POSITION 0
#define _IOC_TYPE_POSITION (_IOC_NUMBER_POSITION + _IOC_NUMBER_SIZE)
#define _IOC_SIZE_POSITION (_IOC_TYPE_POSITION + _IOC_TYPE_SIZE)
#define _IOC_DIRECTION_POSITION (_IOC_SIZE_POSITION + _IOC_SIZE_SIZE)

#define _IOC_NUMBER_MASK ((1 << _IOC_NUMBER_SIZE) - 1)
#define _IOC_TYPE_MASK ((1 << _IOC_TYPE_SIZE) - 1)
#define _IOC_SIZE_MASK ((1 << _IOC_SIZE_SIZE) - 1)
#define _IOC_DIRECTION_MASK ((1 <<_IOC_DIRECTION_SIZE) - 1)


#define _IOC(direction, type, number, size) \
    ((uint32_t) \
        (((direction) << _IOC_DIRECTION_POSITION) \
        | ((type) << _IOC_TYPE_POSITION) \
        | ((number) << _IOC_NUMBER_POSITION) \
        | ((size) << _IOC_SIZE_POSITION) \
    ))

#define _IO(type, number) \
    (_IOC(_IOC_DIRECTION_NONE, type, number, 0))

#define _IOW(type, number, size) \
    (_IOC(_IOC_DIRECTION_WRITE, type, number, sizeof(size)))

#define _IOR(type, number, size) \
    (_IOC(_IOC_DIRECTION_READ, type, number, sizeof(size)))

#define _IOWR(type, number, size) \
    (_IOC((_IOC_DIRECTION_READ | _IOC_DIRECTION_WRITE), type, number, sizeof(size)))

#define _IOC_DIRECTION(call) \
    (((call) >> _IOC_DIRECTION_POSITION) & _IOC_DIRECTION_MASK)

#define _IOC_TYPE(call) \
    (((call) >> _IOC_TYPE_POSITION) & _IOC_TYPE_MASK)

#define _IOC_NUMBER(call) \
    (((call) >> _IOC_NUMBER_POSITION) & _IOC_NUMBER_MASK)

#define _IOC_SIZE(call) \
    (((call) >> _IOC_SIZE_POSITION) & _IOC_SIZE_MASK)

extern "C"
{
    int _ioctl(int fd, uint32_t cmd, void* arg);
    #define ioctl _ioctl
}

