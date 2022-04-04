# Install script for directory: C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/AudioDevice.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/AudioStream.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/BoundingBox.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Camera2D.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Camera3D.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Color.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Font.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Functions.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Gamepad.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Image.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Material.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Matrix.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Mesh.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Model.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/ModelAnimation.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Mouse.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Music.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/physac.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Physics.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Ray.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/RayCollision.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/RaylibException.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/raylib-cpp-utils.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/raylib-cpp.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/raylib.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/raymath.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Rectangle.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/RenderTexture.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Shader.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Sound.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Texture.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Vector2.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Vector3.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Vector4.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/VrStereoConfig.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Wave.hpp"
    "C:/Users/spyid/Documents/NetworkingAssignment/SampleGame/out/build/x64-Debug/_deps/raylib-cpp-src/include/Window.hpp"
    )
endif()

