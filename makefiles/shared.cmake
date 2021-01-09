Set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING 
	"Only do Release and Debug" 
	FORCE
)

Set(SOURCESDK_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/libraries/hl2sdk-csgo)

Set(SOURCESDK            ${SOURCESDK_DIR}/${BRANCH})
Set(SOURCESDK_LIB        ${SOURCESDK}/lib)

include_directories(
  ${SOURCESDK}
  ${SOURCESDK}/common
  ${SOURCESDK}/common/protobuf-2.5.0/src
  ${SOURCESDK}/game/shared
  ${SOURCESDK}/game/server
  ${SOURCESDK}/public
  ${SOURCESDK}/public/engine
  ${SOURCESDK}/public/mathlib
  ${SOURCESDK}/public/tier0
  ${SOURCESDK}/public/tier1
  ${SOURCESDK}/public/engine/protobuf
  libraries/dyncall/dynload
  libraries/asmjit/src
  libraries/DynamicHooks/src
  libraries/tl
  libraries/spdlog/include
  libraries/mono/include/mono-2.0
  libraries
)

SET(ASMJIT_STATIC 1)

Project(Testing C CXX)