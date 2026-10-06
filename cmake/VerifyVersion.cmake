if(NOT DEFINED PROJECT_SOURCE_DIR)
  message(FATAL_ERROR "PROJECT_SOURCE_DIR is required")
endif()

file(READ "${PROJECT_SOURCE_DIR}/CMakeLists.txt" project_cmake)
file(READ "${PROJECT_SOURCE_DIR}/src/app/config.h" config_header)
file(READ "${PROJECT_SOURCE_DIR}/resources/SampleGrabber-macOS-Info.plist" macos_plist)

function(require_match content pattern description)
  if(NOT "${content}" MATCHES "${pattern}")
    message(FATAL_ERROR "Version consistency check failed: ${description}")
  endif()
endfunction()

require_match("${project_cmake}" "project\\(SampleGrabber VERSION 0\\.1\\.0" "CMake project version must be 0.1.0")
require_match("${config_header}" "PLUG_VERSION_HEX 0x00000100" "iPlug2 packed version must be 0x00000100")
require_match("${config_header}" "PLUG_VERSION_STR \"0\\.1\\.0\"" "iPlug2 version string must be 0.1.0")
require_match("${macos_plist}" "<key>CFBundleShortVersionString</key>[ \t\r\n]*<string>0\\.1\\.0</string>" "macOS short version must be 0.1.0")
require_match("${macos_plist}" "<key>CFBundleVersion</key>[ \t\r\n]*<string>0\\.1\\.0</string>" "macOS bundle version must be 0.1.0")
