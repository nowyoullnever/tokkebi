cmake_minimum_required(VERSION 3.21)

project(tokkebi VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

set(IPLUG2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/iPlug2" CACHE PATH "Path to the pinned iPlug2 checkout")
if(NOT EXISTS "${IPLUG2_DIR}/iPlug2.cmake")
  message(FATAL_ERROR "iPlug2 is missing. Run: git submodule update --init --recursive")
endif()

set(IPLUG2_CXX_STANDARD 20 CACHE STRING "C++ standard used by iPlug2 and tokkebi" FORCE)
include("${IPLUG2_DIR}/iPlug2.cmake")
find_package(iPlug2 REQUIRED)

set(TOKKEBI_VST3_SDK_DIR "${IPLUG2_DIR}/Dependencies/IPlug/VST3_SDK")
if(NOT EXISTS "${TOKKEBI_VST3_SDK_DIR}/pluginterfaces/vst/ivstaudioprocessor.h")
  message(FATAL_ERROR
    "tokkebi requires the pinned VST3 SDK at ${TOKKEBI_VST3_SDK_DIR}. "
    "Run: pwsh -File scripts/setup-vst3-sdk.ps1")
endif()

set(TOKKEBI_FORMATS APP VST3)
if(APPLE)
  list(APPEND TOKKEBI_FORMATS AU)
endif()

iplug_add_plugin(tokkebi
  SOURCES
    src/app/tokkebi.cpp
    src/app/tokkebi.h
    src/app/config.h
    src/app/resource.h
  FORMATS ${TOKKEBI_FORMATS}
)

foreach(tokkebi_target IN ITEMS tokkebi-app tokkebi-vst3 tokkebi-au)
  if(TARGET ${tokkebi_target})
    target_include_directories(${tokkebi_target} PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/src/app"
    )
  endif()
endforeach()

if(WIN32 AND TARGET tokkebi-app)
  set_target_properties(tokkebi-app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/out/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/out/Release"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/out/RelWithDebInfo"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/out/MinSizeRel"
  )
endif()

if(WIN32 AND TARGET tokkebi-vst3)
  set_target_properties(tokkebi-vst3 PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/out/Debug/tokkebi.vst3/Contents/x86_64-win"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/out/Release/tokkebi.vst3/Contents/x86_64-win"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/out/RelWithDebInfo/tokkebi.vst3/Contents/x86_64-win"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/out/MinSizeRel/tokkebi.vst3/Contents/x86_64-win"
  )
  add_custom_command(TARGET tokkebi-vst3 POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:tokkebi-vst3>/../Resources"
    COMMENT "Creating configuration-specific VST3 Resources directory"
  )
endif()

foreach(tokkebi_target IN ITEMS tokkebi-app tokkebi-vst3 tokkebi-au)
  if(TARGET ${tokkebi_target})
    if(MSVC)
      target_compile_options(${tokkebi_target} PRIVATE /W4 /permissive-)
    else()
      target_compile_options(${tokkebi_target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
  endif()
endforeach()

if(TARGET tokkebi-vst3)
  add_custom_target(tokkebi-verify-vst3-bundle
    COMMAND "${CMAKE_COMMAND}"
      "-DBUNDLE_ROOT=${CMAKE_BINARY_DIR}/out/$<CONFIG>"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/VerifyWindowsVST3Bundle.cmake"
    DEPENDS tokkebi-vst3
    VERBATIM
  )
endif()

if(BUILD_TESTING)
  add_test(
    NAME tokkebi.IdentityAndVersionConsistency
    COMMAND "${CMAKE_COMMAND}"
      "-DPROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/VerifyVersion.cmake"
  )
endif()
