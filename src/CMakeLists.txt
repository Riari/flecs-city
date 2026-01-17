add_library(CoreDependencies INTERFACE)

target_include_directories(CoreDependencies INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Public)

target_link_libraries(CoreDependencies
    INTERFACE
    flecs::flecs
    glfw
    raylib
    unofficial::enet::enet
)

if(WIN32)
    target_link_libraries(CoreDependencies INTERFACE winmm ws2_32)
    target_compile_definitions(CoreDependencies INTERFACE
        WIN32_LEAN_AND_MEAN
        NOGDI
        NOUSER
    )
endif()

set(SOURCES
    main.cpp
    Application/Application.cpp
    Environment/Options.cpp
    Network/ClientThread.cpp
    Network/NetworkThread.cpp
    Network/ServerThread.cpp
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

