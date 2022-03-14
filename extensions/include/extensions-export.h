#if __has_include("cmark-gfm-extensions_export.h")
#include "cmark-gfm-extensions_export.h"
#else

#ifndef CMARK_GFM_EXTENSIONS_EXPORT_H
#define CMARK_GFM_EXTENSIONS_EXPORT_H

#ifdef CMARK_GFM_EXTENSIONS_STATIC_DEFINE
#  define CMARK_GFM_EXTENSIONS_EXPORT
#  define CMARK_GFM_EXTENSIONS_NO_EXPORT
#else
#  if defined(_WIN32)
#    ifndef CMARK_GFM_EXTENSIONS_EXPORT
#      ifdef libcmark_gfm_extensions_EXPORTS
#        define CMARK_GFM_EXTENSIONS_EXPORT __declspec(dllexport)
#      else
#        define CMARK_GFM_EXTENSIONS_EXPORT __declspec(dllimport)
#      endif
#    endif

#    ifndef CMARK_GFM_EXTENSIONS_NO_EXPORT
#      define CMARK_GFM_EXTENSIONS_NO_EXPORT
#    endif
#  else
#    ifndef CMARK_GFM_EXTENSIONS_EXPORT
#      ifdef libcmark_gfm_extensions_EXPORTS
#        define CMARK_GFM_EXTENSIONS_EXPORT __attribute__((__visibility__("default")))
#      else
#        define CMARK_GFM_EXTENSIONS_EXPORT __attribute__((__visibility__("default")))
#      endif
#    endif

#    ifndef CMARK_GFM_EXTENSIONS_NO_EXPORT
#      define CMARK_GFM_EXTENSIONS_NO_EXPORT __attribute__((__visibility__("hidden")))
#    endif
#  endif
#endif

#ifndef CMARK_GFM_EXTENSIONS_DEPRECATED
#  if !defined(_WIN32)
#    define CMARK_GFM_EXTENSIONS_DEPRECATED __attribute__ ((__deprecated__))
#  else
#    define CMARK_GFM_EXTENSIONS_DEPRECATED
#  endif
#endif

#ifndef CMARK_GFM_EXTENSIONS_DEPRECATED_EXPORT
#  define CMARK_GFM_EXTENSIONS_DEPRECATED_EXPORT CMARK_GFM_EXTENSIONS_EXPORT CMARK_GFM_EXTENSIONS_DEPRECATED
#endif

#ifndef CMARK_GFM_EXTENSIONS_DEPRECATED_NO_EXPORT
#  define CMARK_GFM_EXTENSIONS_DEPRECATED_NO_EXPORT CMARK_GFM_EXTENSIONS_NO_EXPORT CMARK_GFM_EXTENSIONS_DEPRECATED
#endif

#endif /* CMARK_GFM_EXTENSIONS_EXPORT_H */

#endif /* "cmark-gfm-extensions_export.h" */
