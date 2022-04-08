# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.20.21032501-MSVC_2)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(raylib-cpp-populate NONE)



include(ExternalProject)
ExternalProject_Add(raylib-cpp-populate
                     "UPDATE_DISCONNECTED" "False" "URL" "https://github.com/RobLoach/raylib-cpp/archive/master.tar.gz"
                    SOURCE_DIR          "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug-Server/_deps/raylib-cpp-src"
                    BINARY_DIR          "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug-Server/_deps/raylib-cpp-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)


