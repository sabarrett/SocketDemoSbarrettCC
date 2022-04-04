cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(SampleGame)

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
# find_library(RedLib RedLib)
# add_subdirectory(../RedLib ../RedLib-build)

# RedEngine
find_library(RedEngine RedEngine)
add_subdirectory(../RedEngine ../RedEngine-build)

# RedNet
find_library(RedNet RedNet)
add_subdirectory(../RedNet ../RedNet-build)

include_directories("Header Files" "../RedEngine/Header Files" "../RedLib/Header Files" "../RedNet/Header Files")

set(PROJECT_NAME SampleGame)
set(PROJECT_SOURCES "Source Files/Unit.cpp" "Source Files/Game.cpp" "Source Files/main.cpp" "Source Files/Player.cpp" "Source Files/UnitManager.cpp" "Source Files/PlayerMoveEvent.cpp" "Source Files/GameListener.cpp")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
set(raylib_VERBOSE 1)
target_include_directories(RedEngine PUBLIC ../RedLib ../RedLib-build ../RedEngine ../RedEngine-build ../RedNet ../RedNet-build)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib RedLib RedEngine RedNet)
# That's it! You should have an example executable that you can run. Have fun!
