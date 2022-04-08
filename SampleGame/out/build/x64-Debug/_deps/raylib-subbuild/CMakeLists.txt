# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.17.20032601-MSVC_2)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(raylib-populate NONE)

include(ExternalProject)
ExternalProject_Add(raylib-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/raysan5/raylib.git" "GIT_TAG" "b6c8d343dca2ef19c23c50975328a028124cf3cb"
                    SOURCE_DIR          "C:/Users/brandon.labbe/Desktop/NetworkingAssignment1/SampleGame/out/build/x64-Debug/_deps/raylib-src"
                    BINARY_DIR          "C:/Users/brandon.labbe/Desktop/NetworkingAssignment1/SampleGame/out/build/x64-Debug/_deps/raylib-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)
