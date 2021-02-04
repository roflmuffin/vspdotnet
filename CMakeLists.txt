cmake_minimum_required(VERSION 3.18)

Project(vspdotnet C CXX)

include("makefiles/shared.cmake")

add_subdirectory(libraries/spdlog)
add_subdirectory(libraries/dyncall)
add_subdirectory(libraries/asmjit)
add_subdirectory(libraries/DynamicHooks/src)

SET(NATIVES_SOURCES
  src/core/natives/natives_events.cpp
  src/core/natives/natives_commands.cpp
  src/core/natives/natives_conversions.cpp
  src/core/natives/natives_convars.cpp
  src/core/natives/natives_callbacks.cpp
  src/core/natives/natives_entity.cpp
  src/core/natives/natives_engine.cpp
  src/core/natives/natives_vector.cpp
  src/core/natives/natives_player.cpp
  src/core/natives/natives_stringtables.cpp
  src/core/natives/natives_memory.cpp
  src/core/natives/natives_usermessages.cpp
  src/core/natives/natives_menus.cpp
  src/core/natives/natives_timers.cpp
)

set(CONVERSIONS_SOURCES
  src/core/utilities/conversions/address_from.cpp
  src/core/utilities/conversions/baseentity_from.cpp
  src/core/utilities/conversions/basehandle_from.cpp
  src/core/utilities/conversions/edict_from.cpp
  src/core/utilities/conversions/index_from.cpp
  src/core/utilities/conversions/inthandle_from.cpp
  src/core/utilities/conversions/playerinfo_from.cpp
  src/core/utilities/conversions/uniqueid_from.cpp
  src/core/utilities/conversions/userid_from.cpp
)

set(CONVERSIONS_HEADERS
  src/core/utilities/conversions.h    
)

SET(SOURCE_FILES
  "src/core/main.cpp"
  "src/core/interfaces.cpp"
  "src/core/globals.h"
  "src/core/globals.cpp"
  "src/core/log.cpp"
  src/core/sig_scanner.h
  src/core/sig_scanner.cpp
  "src/core/entity_listener.cpp"
  src/core/script_engine.h
  src/core/script_engine.cpp
  src/core/utils.h
  src/core/hooks.h
  src/core/convar_hack.h
  src/core/datamaps.h
  src/core/props.h
  src/core/entity_listener.h
  src/core/interfaces.h
  src/core/log.h
  src/core/main.h
  "${SOURCESDK}/public/engine/protobuf/netmessages.pb.cc"
  "${SOURCESDK}/public/game/shared/csgo/protobuf/cstrike15_usermessages.pb.cc"
  "${SOURCESDK}/public/game/shared/csgo/protobuf/cstrike15_usermessage_helpers.cpp"
  src/core/dotnet_host.h 
  src/core/dotnet_host.cpp 
  src/core/autonative.h
  src/core/autonative.cpp
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
  src/core/menu_manager.h
  src/core/menu_manager.cpp
  src/core/convar_manager.h
  src/core/convar_manager.cpp
  src/core/player_manager.h
  src/core/player_manager.cpp
  src/core/function.h
  src/core/function.cpp
  src/core/timer_system.h
  src/core/timer_system.cpp
  src/core/chat_commands.h
  src/core/chat_commands.cpp
  src/core/hook_manager.h
  src/core/hook_manager.cpp
  "libraries/sourcehook/sourcehook.cpp"
  "libraries/sourcehook/sourcehook_impl_chookidman.cpp"
  "libraries/sourcehook/sourcehook_impl_chookmaninfo.cpp"
  "libraries/sourcehook/sourcehook_impl_cvfnptr.cpp" 
  "libraries/sourcehook/sourcehook_impl_cproto.cpp"
  )

Source_Group("Header Files\\Conversions" FILES ${CONVERSIONS_HEADERS})


Source_Group("Source Files\\Natives" FILES ${NATIVES_SOURCES})
Source_Group("Source Files\\Conversions" FILES ${CONVERSIONS_SOURCES})

# Sources
add_library(${PROJECT_NAME} MODULE ${SOURCE_FILES} ${NATIVES_SOURCES} ${CONVERSIONS_SOURCES} ${CONVERSIONS_HEADERS})

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

add_subdirectory(test)

# Libraries 
#target_link_libraries(${PROJECT_NAME} ${HL2_LIBS} spdlog dynload_s asmjit DynamicHooks /home/michael/csgo_server/csgo/addons/csgodotnet/${CMAKE_CURRENT_SOURCE_DIR}/libraries/mono/lib/libmono-2.0.so)
#target_link_libraries(${PROJECT_NAME} ${HL2_LIBS} spdlog dynload_s asmjit DynamicHooks ${PROJECT_SOURCE_DIR}/libraries/mono/lib/libmono-2.0.so)
target_link_libraries(${PROJECT_NAME} ${VSPDOTNET_LINK_LIBRARIES})

