add_library(Test SHARED
    Test.cpp
)

set_target_properties(Test PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER Test.h
)

target_compile_definitions(Test
    PRIVATE
        TEST_EXPORTS
)

target_link_libraries(Test
    PRIVATE
        CoreDependencies
        Core
)

target_include_directories(Test
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE 
        ../
)
