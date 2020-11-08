include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(TEST_script_engine test_main.cpp
${PROJECT_SOURCE_DIR}/src/core/script_engine.cpp
${PROJECT_SOURCE_DIR}/src/core/log.cpp
)

target_include_directories(
  TEST_script_engine PUBLIC
  ../../src
  )

target_link_libraries(TEST_script_engine ${VSPDOTNET_LINK_LIBRARIES})