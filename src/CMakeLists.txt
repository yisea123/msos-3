set (target_name "msos")

add_executable(msos)

target_sources(msos
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_subdirectory(dynamic_linker)
add_subdirectory(utils)

target_link_libraries(msos
    PUBLIC
        # hal
        msos_dynamic_linker
)

include (GenerateBinary)
generate_binary(${target_name} ${vendor})

target_include_directories(msos
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(modules/example)

set_target_properties(
    msos
    PROPERTIES
        SUFFIX ".elf"
)

target_compile_options(msos PUBLIC "-g"  "-Og" "-ggdb3")