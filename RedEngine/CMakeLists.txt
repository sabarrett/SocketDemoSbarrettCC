cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(RedEngine)

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
        FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG b6c8d343dca2ef19c23c50975328a028124cf3cb
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
        set(BUILD_TESTING  OFF CACHE BOOL "" FORCE)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# raylib-cpp
find_package(raylib-cpp QUIET)
if (NOT raylib-cpp_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib-cpp
        URL https://github.com/RobLoach/raylib-cpp/archive/master.tar.gz
    )
    FetchContent_GetProperties(raylib-cpp)
    if (NOT raylib-cpp_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib-cpp)
        set(BUILD_RAYLIB_CPP_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_TESTING             OFF CACHE BOOL "" FORCE)
        add_subdirectory(${raylib-cpp_SOURCE_DIR} ${raylib-cpp_BINARY_DIR})
    endif()
endif()

# RedLib
find_library(RedLib RedLib)
add_subdirectory(../RedLib ../RedLib-build)

include_directories("Header Files" "../RedLib/Header Files")

# This is the main part:
set(PROJECT_NAME RedEngine)
set(PROJECT_SOURCES "Source Files/Animation.cpp" "Source Files/AnimationData.cpp" "Source Files/AnimationManager.cpp" "Source Files/Color.cpp" "Source Files/Event.cpp" "Source Files/EventListener.cpp" "Source Files/EventSystem.cpp" "Source Files/KeyboardEvent.cpp" "Source Files/MouseEvent.cpp" "Source Files/GraphicsBuffer.cpp" "Source Files/GraphicsBufferManager.cpp" "Source Files/GraphicsSystem.cpp" "Source Files/InputSystem.cpp" "Source Files/Sprite.cpp" "Source Files/Camera2D.cpp")
add_library(${PROJECT_NAME} ${PROJECT_SOURCES})
set(raylib_VERBOSE 1)
target_include_directories(RedEngine PUBLIC ../RedLib ../RedLib-build)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib RedLib)
# That's it! You should have an example executable that you can run. Have fun!
