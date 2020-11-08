include("makefiles/shared.cmake")

SET(_ITERATOR_DEBUG_LEVEL 2)
Add_Definitions(-DCOMPILER_MSVC -DCOMPILER_MSVC32 -D_WIN32 -D_WINDOWS -D_ALLOW_KEYWORD_MACROS -D__STDC_LIMIT_MACROS)

Set(CMAKE_CXX_FLAGS_RELEASE "/D_NDEBUG /MD /wd4005 /MP")

add_compile_options("/MP")
target_compile_options(vspdotnet PRIVATE "$<$<CONFIG:Debug>:/MDd>")
target_link_options(vspdotnet PRIVATE "/SAFESEH:NO")

Set_Target_Properties(vspdotnet PROPERTIES
    LINK_FLAGS_RELEASE "/NODEFAULTLIB:LIBC.lib /NODEFAULTLIB:LIBCMT.lib /NODEFAULTLIB:LIBCPMT.lib"
)

#target_compile_options(Testing PRIVATE "/MD$<$<CONFIG:Release>:d>")

SET(
    VSPDOTNET_LINK_LIBRARIES
    ${SOURCESDK_LIB}/public/tier0.lib
    ${SOURCESDK_LIB}/public/tier1.lib
    ${SOURCESDK_LIB}/public/tier2.lib
    ${SOURCESDK_LIB}/public/tier3.lib
    ${SOURCESDK_LIB}/public/vstdlib.lib
    ${SOURCESDK_LIB}/public/mathlib.lib
    ${SOURCESDK_LIB}/public/interfaces.lib
    ${SOURCESDK_LIB}/win32/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/vs2019/libprotobuf.lib
    legacy_stdio_definitions.lib
    spdlog 
    dynload_s
    dyncall_s
    asmjit 
    DynamicHooks
)