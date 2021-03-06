# This file is part of MSOS project. This is simple OS for embedded development devices.
# Copyright (C) 2019 Mateusz Stadnik
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

function (execute_command command_to_execute working_directory)
    message (STATUS "Executing: `${command_to_execute}` inside ${working_directory}")
    find_package(Git QUIET)
    if (NOT GIT_FOUND)
        message (FATAL_ERROR "Can't find git")
    endif ()

    string(REPLACE " " ";" command_to_execute "${command_to_execute}")

    execute_process(
        COMMAND
            ${command_to_execute}
        WORKING_DIRECTORY
            ${working_directory}
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error
        RESULT_VARIABLE result
        OUTPUT_QUIET
        ERROR_QUIET
    )

    if (NOT result EQUAL "0")
        message ("Failure: ${output}")
        message (FATAL_ERROR "Failure: ${error}")
    endif ()
endfunction()

function (fetch_module_via_tag_or_branch module_name module_path working_directory tag branch)
    message (STATUS "Update module: ${module_name}, with path: ${module_path}, inside: ${working_directory}")

    find_package(Git QUIET)
    if (NOT GIT_FOUND)
        message (FATAL_ERROR "Can't find git")
    endif ()


    string(FIND ${module_name} "/" module_name_last_slash REVERSE)
    string(LENGTH ${module_name} module_length)
    math(EXPR target_name_begin "${module_name_last_slash} + 1")
    math(EXPR target_name_length "${module_length} - ${target_name_begin}")
    string(SUBSTRING ${module_name} ${target_name_begin} ${target_name_length} target_name)

    if (NOT TARGET ${target_name})
        if (DEFINED ENV{NO_DEPS_UPDATE} OR DEFINED NO_DEPS_UPDATE)
            message (STATUS "Updating dependencies disabled!")
            add_subdirectory(${module_path})
            return()
        endif()


        execute_process(
            COMMAND
                git submodule update --init -- ${module_name}
            WORKING_DIRECTORY
                ${working_directory}
            OUTPUT_VARIABLE output
            ERROR_VARIABLE error
            RESULT_VARIABLE result
            OUTPUT_QUIET
            ERROR_QUIET

        )

        if (NOT result EQUAL "0")
            message ("Failure: ${output}")
            message (FATAL_ERROR "Failure: ${error}")
        endif ()

        if (NOT ${module_path} STREQUAL "")
            if (NOT ${branch} STREQUAL "")
                execute_command("git checkout ${branch}" "${working_directory}/${module_name}")
                execute_command("git pull" "${working_directory}/${module_name}")
                add_subdirectory(${module_path})
            elseif (NOT ${tag} STREQUAL "")
                execute_command("git fetch" "${PROJECT_SOURCE_DIR}/${module_name}")
                execute_command("git checkout ${tag}" "${working_directory}/${module_name}")
                add_subdirectory(${module_path})
            endif ()
        endif ()
    endif ()
endfunction()

function (fetch_module_via_tag module_name module_path working_directory tag)
    fetch_module_via_tag_or_branch(${module_name} ${module_path} ${working_directory} ${tag} "")
endfunction()

function (fetch_module_via_branch module_name module_path working_directory branch)
    fetch_module_via_tag_or_branch(${module_name} ${module_path} ${working_directory} "" ${branch})
endfunction()

