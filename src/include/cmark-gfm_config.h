#ifndef CMARK_CONFIG_H
#define CMARK_CONFIG_H

#ifdef CMARK_USE_CMAKE_HEADERS
// if the CMake config header exists, use that instead of this Swift package prebuilt one
// we need to undefine the header guard, since config.h uses the same one
#undef CMARK_CONFIG_H
#include "config.h"
#else

#ifdef __cplusplus
extern "C" {
#endif

#if __STDC_VERSION__ >= 199901l
  #include <stdbool.h>
#elif !defined(__cplusplus)
  typedef char bool;
#endif

#define CMARK_THREADING

#ifdef __cplusplus
}
#endif

#endif /* not CMARK_USE_CMAKE_HEADERS */

#endif /* not CMARK_CONFIG_H */
