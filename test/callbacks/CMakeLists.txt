include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(TEST_callbacks test_main.cpp
${PROJECT_SOURCE_DIR}/src/core/global_listener.h
${PROJECT_SOURCE_DIR}/src/core/script_engine.cpp
${PROJECT_SOURCE_DIR}/src/core/callback_manager.cpp
${PROJECT_SOURCE_DIR}/src/core/log.cpp
)

target_include_directories(
  TEST_callbacks PUBLIC
  ../../src
  )

target_link_libraries(TEST_callbacks ${VSPDOTNET_LINK_LIBRARIES})