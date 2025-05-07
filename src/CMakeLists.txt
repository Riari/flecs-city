add_library(CoreDependencies INTERFACE)

target_include_directories(CoreDependencies INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Public)

target_link_libraries(CoreDependencies
    INTERFACE
        flecs::flecs
        raylib
        spdlog::spdlog
        GameNetworkingSockets::shared
)

set(SOURCES
    main.cpp
    Environment/Options.cpp
    Network/Server.cpp
    Network/Client.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
        Modules
)

file(GLOB MODULE_DIRS "Modules/*")
foreach(MODULE_DIR ${MODULE_DIRS})
    if(IS_DIRECTORY ${MODULE_DIR})
        add_subdirectory(${MODULE_DIR})
        get_filename_component(MODULE_NAME ${MODULE_DIR} NAME)
        list(APPEND MODULES ${MODULE_NAME})
    endif()
endforeach()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        taywee::args
        CoreDependencies
        ${MODULES}
)

