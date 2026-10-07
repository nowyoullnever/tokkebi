if(NOT DEFINED PROJECT_SOURCE_DIR OR NOT DEFINED VALIDATOR_SCRIPT)
  message(FATAL_ERROR "PROJECT_SOURCE_DIR and VALIDATOR_SCRIPT are required")
endif()

set(fixture_root "${CMAKE_CURRENT_BINARY_DIR}/tokkebi-identity-validator-fixture")
file(REMOVE_RECURSE "${fixture_root}")
file(MAKE_DIRECTORY "${fixture_root}/src/app" "${fixture_root}/resources" "${fixture_root}/docs" "${fixture_root}/assets/fonts")

function(write_valid_fixture)
  file(WRITE "${fixture_root}/CMakeLists.txt" "project(tokkebi VERSION 0.1.0)\nFORMATS \${TOKKEBI_FORMATS}\n")
  file(WRITE "${fixture_root}/AGENTS.md" "# fixture\n")
  file(WRITE "${fixture_root}/MASTER_BLUEPRINT.md" "# tokkebi\n")
  file(WRITE "${fixture_root}/docs/nested.md" "# tokkebi documentation\n")
  file(WRITE "${fixture_root}/src/app/config.h" [=[
#define PLUG_NAME "tokkebi"
#define PLUG_VERSION_HEX 0x00000100
#define PLUG_VERSION_STR "0.1.0"
#define PLUG_UNIQUE_ID 'Tkb1'
#define PLUG_MFR_ID 'NYN1'
#define PLUG_URL_STR "https://github.com/nowyoullnever/tokkebi"
#define PLUG_CHANNEL_IO "0-2"
#define PLUG_CHANNEL_IO "2-2"
]=])
  file(WRITE "${fixture_root}/resources/main.rc" [=[
VALUE "ProductName", "tokkebi"
VALUE "FileVersion", "0.1.0"
]=])
  set(plist_common "<key>CFBundleExecutable</key><string>tokkebi</string>\n<key>CFBundleName</key><string>tokkebi</string>\n<key>CFBundleShortVersionString</key><string>0.1.0</string>\n<key>CFBundleVersion</key><string>0.1.0</string>\n<key>CFBundleSignature</key><string>Tkb1</string>\n")
  file(WRITE "${fixture_root}/resources/tokkebi-macOS-Info.plist" "<key>CFBundleIdentifier</key><string>com.nowyoullnever.tokkebi</string>\n${plist_common}")
  file(WRITE "${fixture_root}/resources/tokkebi-VST3-Info.plist" "<key>CFBundleIdentifier</key><string>com.nowyoullnever.vst3.tokkebi</string>\n${plist_common}")
  file(WRITE "${fixture_root}/resources/tokkebi-AU-Info.plist" "<key>CFBundleIdentifier</key><string>com.nowyoullnever.audiounit.tokkebi</string>\n${plist_common}")
  file(APPEND "${fixture_root}/resources/tokkebi-AU-Info.plist" "<key>manufacturer</key><string>NYN1</string>\n<key>subtype</key><string>Tkb1</string>\n<key>type</key><string>aufx</string>\n")
endfunction()

function(run_validator test_name should_pass expected_message)
  execute_process(
    COMMAND "${CMAKE_COMMAND}" "-DPROJECT_SOURCE_DIR=${fixture_root}" -P "${VALIDATOR_SCRIPT}"
    RESULT_VARIABLE validator_result
    OUTPUT_VARIABLE validator_stdout
    ERROR_VARIABLE validator_stderr
  )
  set(validator_output "${validator_stdout}${validator_stderr}")
  if(should_pass AND NOT validator_result EQUAL 0)
    message(FATAL_ERROR "${test_name} should pass, but failed:\n${validator_output}")
  endif()
  if(NOT should_pass AND validator_result EQUAL 0)
    message(FATAL_ERROR "${test_name} should fail, but passed")
  endif()
  if(NOT should_pass AND NOT validator_output MATCHES "${expected_message}")
    message(FATAL_ERROR "${test_name} failed for an unexpected reason:\n${validator_output}")
  endif()
endfunction()

write_valid_fixture()
run_validator("Case D: correct branding and metadata" TRUE "")

# Case C: copy a real executable as a font-like binary asset. The extension is
# deliberately outside the text allowlist, so the validator must not read it.
file(COPY_FILE "${CMAKE_COMMAND}" "${fixture_root}/assets/fonts/fixture.ttf")
run_validator("Case C: binary font fixture is ignored" TRUE "")

file(WRITE "${fixture_root}/docs/nested.md" "SAMPLEGRABBER\n")
run_validator("Case A: uppercase legacy branding" FALSE "Obsolete active product identifier")

file(WRITE "${fixture_root}/docs/nested.md" "SaMpLeGrAbBeR\n")
run_validator("Case B: mixed-case legacy branding" FALSE "Obsolete active product identifier")

file(WRITE "${fixture_root}/docs/nested.md" "# tokkebi documentation\n")
file(READ "${fixture_root}/src/app/config.h" fixture_config)
string(REPLACE "0.1.0" "9.9.9" fixture_config "${fixture_config}")
file(WRITE "${fixture_root}/src/app/config.h" "${fixture_config}")
run_validator("Case E: incorrect plugin version" FALSE "iPlug2 version string")

write_valid_fixture()
file(READ "${fixture_root}/src/app/config.h" fixture_config)
string(REPLACE "https://github.com/nowyoullnever/tokkebi" "https://github.com/example/incorrect" fixture_config "${fixture_config}")
file(WRITE "${fixture_root}/src/app/config.h" "${fixture_config}")
run_validator("Case E: incorrect repository URL" FALSE "repository URL")

file(REMOVE_RECURSE "${fixture_root}")
