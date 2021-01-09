cmake_minimum_required(VERSION 3.18)

Project(Testing C CXX)

include("makefiles/shared.cmake")

add_subdirectory(libraries/spdlog)
add_subdirectory(libraries/dyncall/dynload)
add_subdirectory(libraries/asmjit)
add_subdirectory(libraries/DynamicHooks/src)


SET(SOURCE_FILES
  "src/core/main.cpp"
  "src/core/interfaces.cpp"
  "src/core/globals.cpp"
  "src/core/log.cpp"
  "src/core/sig_scanner.cpp"
  "src/core/entity_listener.cpp"
  src/core/script_engine.h
  src/core/script_engine.cpp
  src/core/utils.h
  "${SOURCESDK}/public/engine/protobuf/netmessages.pb.cc"
  "${SOURCESDK}/public/game/shared/csgo/protobuf/cstrike15_usermessages.pb.cc"
  "${SOURCESDK}/public/game/shared/csgo/protobuf/cstrike15_usermessage_helpers.cpp"
  src/core/dotnet_host.h 
  src/core/dotnet_host.cpp 
  src/core/autonative.h
  src/core/event_manager.h
  src/core/event_manager.cpp
  src/core/user_message_manager.h
  src/core/user_message_manager.cpp
  src/core/entity.h
  src/core/entity.cpp
  src/core/engine_trace.h
  src/core/engine_trace.cpp
  src/core/datamaps.cpp
  src/core/props.cpp
  src/core/con_command_manager.cpp
  src/core/callback_manager.h
  src/core/callback_manager.cpp
  src/core/convar_manager.h
  src/core/convar_manager.cpp
  src/core/player_manager.h
  src/core/player_manager.cpp
  src/core/natives/natives_events.cpp
  src/core/natives/natives_commands.cpp
  src/core/natives/natives_conversions.cpp
  src/core/natives/natives_convars.cpp
  src/core/natives/natives_callbacks.cpp
  src/core/natives/natives_entity.cpp
  src/core/natives/natives_engine.cpp
  src/core/natives/natives_vector.cpp
  src/core/natives/natives_player.cpp
  "src/core/utilities/conversions/address_from.cpp"
  "src/core/utilities/conversions/baseentity_from.cpp"
  "src/core/utilities/conversions/basehandle_from.cpp"
  "src/core/utilities/conversions/edict_from.cpp"
  "src/core/utilities/conversions/index_from.cpp"
  "src/core/utilities/conversions/inthandle_from.cpp"
  "src/core/utilities/conversions/playerinfo_from.cpp"
  "src/core/utilities/conversions/uniqueid_from.cpp"
  "src/core/utilities/conversions/userid_from.cpp"
  "libraries/sourcehook/sourcehook.cpp"
  "libraries/sourcehook/sourcehook_impl_chookidman.cpp"
  "libraries/sourcehook/sourcehook_impl_chookmaninfo.cpp"
  "libraries/sourcehook/sourcehook_impl_cvfnptr.cpp" 
  "libraries/sourcehook/sourcehook_impl_cproto.cpp"
  )

# Sources
add_library(${PROJECT_NAME} MODULE ${SOURCE_FILES})


target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

If(WIN32)
    include("makefiles/win32/win32.base.cmake")
Else()
    include("makefiles/linux/linux.base.cmake")
EndIf()


# Libraries 
#target_link_libraries(${PROJECT_NAME} ${HL2_LIBS} spdlog dynload_s asmjit DynamicHooks /home/michael/csgo_server/csgo/addons/csgodotnet/${CMAKE_CURRENT_SOURCE_DIR}/libraries/mono/lib/libmono-2.0.so)
#target_link_libraries(${PROJECT_NAME} ${HL2_LIBS} spdlog dynload_s asmjit DynamicHooks ${PROJECT_SOURCE_DIR}/libraries/mono/lib/libmono-2.0.so)
target_link_libraries(${PROJECT_NAME} ${TESTING_LINK_LIBRARIES})
