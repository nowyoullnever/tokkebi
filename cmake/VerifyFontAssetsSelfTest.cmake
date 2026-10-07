if(NOT DEFINED PROJECT_SOURCE_DIR OR NOT DEFINED VALIDATOR_SCRIPT)
  message(FATAL_ERROR "PROJECT_SOURCE_DIR and VALIDATOR_SCRIPT are required")
endif()

set(test_root "${CMAKE_CURRENT_BINARY_DIR}/FontAssetsSelfTest")
file(REMOVE_RECURSE "${test_root}")
file(MAKE_DIRECTORY "${test_root}")
file(COPY "${PROJECT_SOURCE_DIR}/assets" DESTINATION "${test_root}")
file(COPY "${PROJECT_SOURCE_DIR}/third_party/licenses" DESTINATION "${test_root}/third_party")

function(expect_manifest_rejection name from to)
  set(case_root "${test_root}/${name}")
  file(REMOVE_RECURSE "${case_root}")
  file(COPY "${test_root}/assets" DESTINATION "${case_root}")
  file(COPY "${test_root}/third_party" DESTINATION "${case_root}")
  set(case_manifest "${case_root}/assets/fonts/manifest.json")
  file(READ "${case_manifest}" contents)
  string(REPLACE "${from}" "${to}" modified_contents "${contents}")
  if(modified_contents STREQUAL contents)
    message(FATAL_ERROR "Self-test ${name} did not modify its manifest fixture")
  endif()
  file(WRITE "${case_manifest}" "${modified_contents}")
  execute_process(
    COMMAND "${CMAKE_COMMAND}" "-DPROJECT_SOURCE_DIR=${case_root}" -P "${VALIDATOR_SCRIPT}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error)
  if(result EQUAL 0)
    message(FATAL_ERROR "Self-test ${name} unexpectedly accepted an invalid manifest")
  endif()
endfunction()

expect_manifest_rejection(
  manifest_hash
  "3f184d949149f2e7e6a853593e25161fb1d3b6d3708ebf79ce78600788fdea30"
  "0000000000000000000000000000000000000000000000000000000000000000")
expect_manifest_rejection(
  manifest_license_path
  "third_party/licenses/fonts/DungGeunMo-Public-Domain.txt"
  "third_party/licenses/fonts/not-present.txt")
