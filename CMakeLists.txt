cmake_minimum_required(VERSION 3.21)

project(SampleGrabber VERSION 0.1.0 LANGUAGES CXX)

set(IPLUG2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/iPlug2" CACHE PATH "Path to the pinned iPlug2 checkout")
if(NOT EXISTS "${IPLUG2_DIR}/iPlug2.cmake")
  message(FATAL_ERROR "iPlug2 is missing. Run: git submodule update --init --recursive")
endif()

set(IPLUG2_CXX_STANDARD 20 CACHE STRING "C++ standard used by iPlug2 and SampleGrabber" FORCE)
include("${IPLUG2_DIR}/iPlug2.cmake")
find_package(iPlug2 REQUIRED)

iplug_add_plugin(SampleGrabber
  SOURCES
    src/app/SampleGrabber.cpp
    src/app/SampleGrabber.h
    src/app/config.h
    src/app/resource.h
  FORMATS APP
)

target_include_directories(SampleGrabber-app PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src/app"
)

if(MSVC)
  target_compile_options(SampleGrabber-app PRIVATE /W4 /permissive-)
else()
  target_compile_options(SampleGrabber-app PRIVATE -Wall -Wextra -Wpedantic)
endif()
