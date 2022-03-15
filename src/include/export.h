#ifndef CMARK_GFM_EXPORT_H
#define CMARK_GFM_EXPORT_H

#ifdef CMARK_USE_CMAKE_HEADERS
// if the CMake config header exists, use that instead of this Swift package prebuilt one
// we need to undefine the header guard, since export.h uses the same one
#undef CMARK_GFM_EXPORT_H
#include "cmark-gfm_export.h"
#else

#ifdef CMARK_GFM_STATIC_DEFINE
#  define CMARK_GFM_EXPORT
#  define CMARK_GFM_NO_EXPORT
#else
#  if defined(_WIN32)
#    if defined(libcmark_gfm_EXPORTS)
#      define CMARK_GFM_EXPORT __declspec(dllexport)
#    else
#      define CMARK_GFM_EXPORT __declspec(dllimport)
#    endif
#    define CMARK_GFM_NO_EXPORT
#  else
#    if defined(libcmark_gfm_EXPORTS)
#      define CMARK_GFM_EXPORT __attribute__((__visibility__("default")))
#    else
#      define CMARK_GFM_EXPORT __attribute__((__visibility__("default")))
#    endif
#    define CMARK_GFM_NO_EXPORT __attribute__((__visibility__("hidden")))
#  endif
#endif

#ifndef CMARK_GFM_DEPRECATED
#  if defined(_WIN32)
#    define CMARK_GFM_DEPRECATED __declspec(deprecated)
#  else
#    define CMARK_GFM_DEPRECATED __attribute__ ((__deprecated__))
#  endif
#endif

#ifndef CMARK_GFM_DEPRECATED_EXPORT
#  define CMARK_GFM_DEPRECATED_EXPORT CMARK_GFM_EXPORT CMARK_GFM_DEPRECATED
#endif

#ifndef CMARK_GFM_DEPRECATED_NO_EXPORT
#  define CMARK_GFM_DEPRECATED_NO_EXPORT CMARK_GFM_NO_EXPORT CMARK_GFM_DEPRECATED
#endif

#endif /* not CMARK_USE_CMAKE_HEADERS */

#endif /* not CMARK_GFM_EXPORT_H */
