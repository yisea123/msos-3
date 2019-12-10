add_executable(host)

target_sources(host
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(host
    PUBLIC
        hal
        msos_dynamic_linker
)

include (GenerateBinary)
generate_binary(host ${vendor})

target_include_directories(host 
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(host 
    PROPERTIES
        SUFFIX ".elf"
)

find_program(qemu_img qemu-img)

if (NOT qemu_img)
    message(FATAL_ERROR "qemu-img executable not found!")
endif ()

add_custom_command(
    TARGET host 
    COMMAND ${qemu_img} resize ${CMAKE_CURRENT_BINARY_DIR}/host.bin 32K
    POST_BUILD
    VERBATIM
)

set (HOST_BINARY_FILE "${CMAKE_CURRENT_BINARY_DIR}/host.bin" CACHE INTERNAL "")

target_compile_options(host 
    PUBLIC 
    $<$<CONFIG:DEBUG>:-Og -g>
    $<$<CONFIG:RELEASE>:-Os>
) 
