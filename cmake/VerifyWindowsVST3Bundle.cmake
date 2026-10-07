if(NOT DEFINED BUNDLE_ROOT)
  message(FATAL_ERROR "BUNDLE_ROOT is required")
endif()

set(bundle_root "${BUNDLE_ROOT}/tokkebi.vst3")
set(binary "${bundle_root}/Contents/x86_64-win/tokkebi.vst3")
set(resources "${bundle_root}/Contents/Resources")

if(NOT EXISTS "${binary}")
  message(FATAL_ERROR "VST3 bundle binary is missing: ${binary}")
endif()

if(NOT IS_DIRECTORY "${resources}")
  message(FATAL_ERROR "VST3 bundle Resources directory is missing: ${resources}")
endif()

message(STATUS "Validated VST3 bundle: ${bundle_root}")
