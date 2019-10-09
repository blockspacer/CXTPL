cmake_minimum_required(VERSION 2.8)
project(chromium)
#set(WTF_LIBRARY_NAME wtf)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#set(BLINK_WTF_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/blink/renderer/platform/)
#set(BLINK_WTF_DIR ${BLINK_WTF_PARENT_DIR}wtf/)

set(BASE_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
set(BASE_DIR ${BASE_PARENT_DIR}base/)

#set(MOJO_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
#set(MOJO_DIR ${MOJO_PARENT_DIR}mojo/)
#
#set(UI_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
#set(UI_DIR ${UI_PARENT_DIR}ui/)
#
#set(UI_COMPOSITOR_PARENT_DIR ${UI_DIR}/)
#set(UI_COMPOSITOR_DIR ${UI_COMPOSITOR_PARENT_DIR}compositor/)
#
#set(UI_DISPLAY_PARENT_DIR ${UI_DIR}/)
#set(UI_DISPLAY_DIR ${UI_DISPLAY_PARENT_DIR}display/)
#
#set(UI_NATIVE_THEME_PARENT_DIR ${UI_DIR}/)
#set(UI_NATIVE_THEME_DIR ${UI_NATIVE_THEME_PARENT_DIR}native_theme/)
#
#set(UI_VIEWS_PARENT_DIR ${UI_DIR}/)
#set(UI_VIEWS_DIR ${UI_VIEWS_PARENT_DIR}views/)
#
#set(UI_GFX_PARENT_DIR ${UI_DIR}/)
#set(UI_GFX_DIR ${UI_GFX_PARENT_DIR}gfx/)
#
#set(UI_EVENTS_PARENT_DIR ${UI_DIR}/)
#set(UI_EVENTS_DIR ${UI_EVENTS_PARENT_DIR}events/)
#
#set(UI_BASE_PARENT_DIR ${UI_DIR}/)
#set(UI_BASE_DIR ${UI_BASE_PARENT_DIR}base/)
#
#set(UI_GL_PARENT_DIR ${UI_DIR}/)
#set(UI_GL_DIR ${UI_GL_PARENT_DIR}gl/)
#
#set(SERVICES_METRICS_PUBLIC_CPP_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/services/metrics/public/)
#set(SERVICES_METRICS_PUBLIC_CPP_DIR ${SERVICES_METRICS_PUBLIC_CPP_PARENT_DIR}cpp/)
#
#set(SERVICES_NETWORK_PUBLIC_CPP_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/services/network/public/)
#set(SERVICES_NETWORK_PUBLIC_CPP_DIR ${SERVICES_NETWORK_PUBLIC_CPP_PARENT_DIR}cpp/)
#
#set(SERVICES_SERVICE_MANAGER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/services/service_manager/)
#set(SERVICES_SERVICE_MANAGER_PUBLIC_DIR ${SERVICES_SERVICE_MANAGER_DIR}/public/)
#set(SERVICES_SERVICE_MANAGER_PUBLIC_CPP_DIR ${SERVICES_SERVICE_MANAGER_PUBLIC_DIR}cpp/)
#
#set(COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/components/)
#
#set(COMPONENTS_VIZ_PARENT_DIR ${COMPONENTS_DIR})
#set(COMPONENTS_VIZ_DIR ${COMPONENTS_VIZ_PARENT_DIR}viz/)
#
#set(COMPONENTS_VIZ_COMMON_DIR ${COMPONENTS_VIZ_DIR}common/)
#
#set(COMPONENTS_VIZ_CLIENT_DIR ${COMPONENTS_VIZ_DIR}client/)
#
#set(COMPONENTS_CONTENT_SETTINGS_CORE_COMMON_PARENT_DIR ${COMPONENTS_DIR}content_settings/core/)
#set(COMPONENTS_CONTENT_SETTINGS_CORE_COMMON_DIR ${COMPONENTS_CONTENT_SETTINGS_CORE_COMMON_PARENT_DIR}common/)
#
#set(libsync_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
#set(libsync_DIR ${libsync_PARENT_DIR}libsync/)
#
#set(khronos_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
#set(khronos_DIR ${khronos_PARENT_DIR}khronos/)
#
#set(GFX_GEOMETRY_PARENT_DIR ${UI_DIR}/gfx/)
#set(GFX_GEOMETRY_DIR ${GFX_GEOMETRY_PARENT_DIR}geometry/)
#
#set(GFX_RANGE_PARENT_DIR ${UI_DIR}/gfx/)
#set(GFX_RANGE_DIR ${GFX_RANGE_PARENT_DIR}range/)
#
#set(GFX_GEOMETRY_SKIA_PARENT_DIR ${UI_DIR}/gfx/)
#set(GFX_GEOMETRY_SKIA_DIR ${GFX_GEOMETRY_SKIA_PARENT_DIR})
#
#set(GFX_COLOR_SPACE_PARENT_DIR ${UI_DIR}/gfx/)
#set(GFX_COLOR_SPACE_DIR ${GFX_COLOR_SPACE_PARENT_DIR})
#
#set(GFX_ANIMATION_PARENT_DIR ${UI_DIR}/gfx/)
#set(GFX_ANIMATION_DIR ${GFX_ANIMATION_PARENT_DIR}/animation/)
#
#set(GFX_CODEC_PARENT_DIR ${UI_DIR}/gfx/)
#set(GFX_CODEC_DIR ${GFX_CODEC_PARENT_DIR}/codec/)
#
#set(GFX_SWITCHES_PARENT_DIR ${UI_DIR}/gfx/)
#set(GFX_SWITCHES_DIR ${GFX_SWITCHES_PARENT_DIR})
#
#set(CC_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
#set(CC_DIR ${CC_PARENT_DIR}cc/)
#
#set(BLINK_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/blink/common/)
#
#set(BLINK_RENDERER_PLATFORM_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
#set(BLINK_RENDERER_PLATFORM_DIR ${BLINK_RENDERER_PLATFORM_PARENT_DIR}blink/renderer/platform/)
#
#set(BLINK_RENDERER_CORE_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
#set(BLINK_RENDERER_CORE_DIR ${BLINK_RENDERER_CORE_PARENT_DIR}blink/renderer/core/)
#
##set(BLINK_RENDERER_NETWORK_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
##set(BLINK_RENDERER_NETWORK_DIR ${BLINK_RENDERER_NETWORK_PARENT_DIR}blink/renderer/platform/network/)
#
#set(GMEDIA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/media/)
#
#set(BLINK_PUBLIC_COMMON_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
#set(BLINK_PUBLIC_COMMON_DIR ${BLINK_PUBLIC_COMMON_PARENT_DIR}blink/public/common/)
#
#set(BLINK_PUBLIC_MOJOM_DIR ${BLINK_PUBLIC_COMMON_PARENT_DIR}blink/public/mojom/)
#
#set(BASE_CC_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cc/)
#set(BASE_CC_DIR ${BASE_CC_PARENT_DIR}base/)
#
#set(DEBUG_CC_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cc/)
#set(DEBUG_CC_DIR ${DEBUG_CC_PARENT_DIR}debug/)
#
#set(PAINT_CC_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cc/)
#set(PAINT_CC_DIR ${PAINT_CC_PARENT_DIR}paint/)
#
#set(ANIMATION_CC_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cc/)
#set(ANIMATION_CC_DIR ${ANIMATION_CC_PARENT_DIR}animation/)
#
#if(ENABLE_BORINGSSL)
#  set(BORINGSSL_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/)
#  set(BORINGSSL_DIR ${BORINGSSL_PARENT_DIR}boringssl/)
#endif(ENABLE_BORINGSSL)
#
#set(GCRYPTO_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
#set(GCRYPTO_DIR ${GCRYPTO_PARENT_DIR}crypto/)

set(xdg_mime_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/base/third_party/)
set(xdg_mime_DIR ${xdg_mime_PARENT_DIR}xdg_mime/)

# if(ENABLE_GNET)
#   set(GNET_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
#   set(GNET_DIR ${GNET_PARENT_DIR}net/)
# endif(ENABLE_GNET)
#
# set(GURL_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
# set(GURL_DIR ${GURL_PARENT_DIR}url/)
#
# set(GPU_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gpu/)
#
# set(GPU_GLES2_DIR_PARENT_DIR ${GPU_DIR}/)
# set(GPU_GLES2_DIR ${GPU_GLES2_PARENT_DIR}GLES2/)

#set(GPU_COMMAND_BUFFER_PARENT_DIR ${GPU_DIR}/)
#set(GPU_COMMAND_BUFFER_DIR ${GPU_COMMAND_BUFFER_PARENT_DIR}command_buffer/)

#set(GPU_COMMAND_BUFFER_ES2_DIR ${GPU_COMMAND_BUFFER_PARENT_DIR}GLES2/)

#set(3RDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)

# Location of build tools dir (required for generating "generated_build_date.h "features.h" and "debugging_flags.h")
#SET(BUILD_TOOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
#message(FATAL_ERROR ${BUILD_TOOLS_DIR})

# requires to set 'GEN_*_DIR' vars
#include(configure_buildflags.cmake)

include(modp_b64.cmake) # TODO: link with modp_b64

#configure_file(cmakeconfig.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmakeconfig.h)
# add_definitions(-DBUILDING_WTF)
# add_definitions(-DSTATICALLY_LINKED_WITH_WTF)
#
# set(WTF_COMMON_DEFINITIONS
#   # see in src/chromium/build/build_config.h
#   # USE_TCMALLOC=0
#   # set in src/chromium/allocator_buildflags.h.inc
#   # USE_NEW_TCMALLOC=1
# )
#
# if(TARGET_EMSCRIPTEN)
#   list(APPEND WTF_COMMON_DEFINITIONS
#     # see src/chromium/build/build_config.h
#     NO_TCMALLOC=1
#   )
#
#   set(WTF_EMCC_DEFINITIONS
#     # see build/build_config.h
#     #OS_ASMJS=1
#     # see build/build_config.h
#     __asmjs__
#     # see build/build_config.h
#     #OS_POSIX=1
#     # DYNAMIC_ANNOTATIONS useful when implementing condition variables such as CondVar,
#     # using conditional critical sections (Await/LockWhen) and when constructing
#     # user-defined synchronization mechanisms.
#     DYNAMIC_ANNOTATIONS_ENABLED=0
#     WTF_USE_DYNAMIC_ANNOTATIONS=0
#     # see src/chromium/build/build_config.h
#     # NO_TCMALLOC=1
#     DISABLE_NACL
#     # POSIX_AVOID_MMAP #BUILDFLAG
#     #USE_EXPERIMENTAL_ALLOCATOR_SHIM=0
#   )
# else()
#   list(APPEND WTF_COMMON_DEFINITIONS
#     # see in src/chromium/build/build_config.h
#     # USE_TCMALLOC=1
#     # see in src/chromium/allocator_buildflags.h.inc
#     # USE_NEW_TCMALLOC=1
#   )
#
#   set(WTF_EMCC_DEFINITIONS "")
# endif ()
#
# #if (TARGET_EMSCRIPTEN)
# #  # libxml
# #  #include(GLIBXML.cmake)
# #  # zlib
# #  include(GZLIB_EXT.cmake)
# #elseif(TARGET_LINUX)
# #  # khronos
# #  if(ENABLE_KHRONOS)
# #    # khronos
# #    include(khronos.cmake)
# #  endif(ENABLE_KHRONOS)
# #  # libsync
# #  set(libsync_LIB
# #    libsync
# #  )
# #  include(libsync.cmake)
# #  # zlib
# #  include(GZLIB.cmake)
# #  # zlib
# #  include(GZLIB_EXT.cmake)
# #else()
# #  message(FATAL_ERROR "platform not supported")
# #endif()

#if(ENABLE_HARFBUZZ AND TARGET_LINUX)
#  # fontconfig
#  include(fontconfig.cmake)
#endif(ENABLE_HARFBUZZ AND TARGET_LINUX)

#if(ENABLE_BORINGSSL)
#  # BORINGSSL
#  include(BORINGSSL.cmake)
#endif(ENABLE_BORINGSSL)

if (NOT EMSCRIPTEN)
  # libevent
  include(libevent.cmake)

  # tcmalloc
  include(tcmalloc.cmake)
endif (NOT EMSCRIPTEN)

# dynamic_annotations
include(dynamic_annotations.cmake)

if(TARGET_LINUX)
  # xdg_mime
  include(xdg_mime.cmake)
  # xdg_user_dirs
  include(xdg_user_dirs.cmake)
endif(TARGET_LINUX)

#
# ICU
#
if(USE_CUSTOM_ICU)
  include(icu.cmake)
endif(USE_CUSTOM_ICU)

#
# HARFBUZZ
#
if(USE_CUSTOM_HARFBUZZ)
  include(harfbuzz.cmake)
endif(USE_CUSTOM_HARFBUZZ)

# GURL: URL parsing.
# Depends on : icu (libicuuc), base.
#include(GURL.cmake)

# GCRYPTO: Cryptographic helper library.
# Depends on : base, libnss or BoringSSL
#
# TODO: use nss, not BoringSSL
# as in https://github.com/Tarnyko/chromium-lite/blob/master/README
#
#include(GCRYPTO.cmake)

# base: General-purpose toolkit
# Depends on : libevent, libicui18n
#
# NOTE: remove fork of libicui
# as in https://github.com/Tarnyko/chromium-lite/blob/master/README#L76
#
include(base.cmake)
