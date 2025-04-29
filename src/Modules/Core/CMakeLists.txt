add_library(Core SHARED
    Core.cpp
    ECS/Phases.cpp
)

set_target_properties(Core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER Core.h
)

target_compile_definitions(Core
    PRIVATE
        CORE_EXPORTS
)

target_link_libraries(Core
    PRIVATE
        CoreDependencies
)

target_include_directories(Core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ../
)
