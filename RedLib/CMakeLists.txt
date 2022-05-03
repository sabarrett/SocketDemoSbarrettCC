cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(RedLib)
include_directories("Header Files" "Source Files")
add_library(RedLib "Source Files/MemoryTracker.cpp" "Source Files/Trackable.cpp" "Source Files/Timer.cpp" "Source Files/Vector2D.cpp")
