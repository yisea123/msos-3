add_library(module_with_data_lib OBJECT)

target_sources(module_with_data_lib
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_compile_options(module_with_data_lib
    PUBLIC 
    $<$<CONFIG:DEBUG>:-Og -g>
    $<$<CONFIG:RELEASE>:-Os>
) 

target_link_libraries(module_with_data_lib
    PUBLIC
        module_flags
        msos_utils
)

add_module(module_with_data module_with_data_lib)

include(TestUtils)
register_system_test_binary(module_with_data)