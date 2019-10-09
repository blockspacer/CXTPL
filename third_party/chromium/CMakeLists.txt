cmake_minimum_required(VERSION 2.8)
project(chromium_base)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(BASE_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
set(BASE_DIR ${BASE_PARENT_DIR}base/)

set(xdg_mime_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/base/third_party/)
set(xdg_mime_DIR ${xdg_mime_PARENT_DIR}xdg_mime/)

include(modp_b64.cmake) # TODO: link with modp_b64

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

# base: General-purpose toolkit
# Depends on : libevent, libicui18n
#
# NOTE: remove fork of libicui
# as in https://github.com/Tarnyko/chromium-lite/blob/master/README#L76
#
include(base.cmake)
