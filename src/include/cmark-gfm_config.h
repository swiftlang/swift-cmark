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

#if defined(__has_include)
#  if __has_include(<unistd.h>)
#    define HAVE_UNISTD_H
#  endif
#elif defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
  // Apple Clang does not define any of the unix symbols, even though it provides unistd.h
#  define HAVE_UNISTD_H
#endif

#define HAVE___BUILTIN_EXPECT

#define CMARK_THREADING

#ifndef CMARK_INLINE
  #if defined(_MSC_VER) && !defined(__cplusplus)
    #define CMARK_INLINE __inline
  #else
    #define CMARK_INLINE inline
  #endif
#endif

/* snprintf and vsnprintf fallbacks for MSVC before 2015,
   due to Valentin Milea http://stackoverflow.com/questions/2915672/
*/

#if defined(_MSC_VER) && _MSC_VER < 1900

#include <stdio.h>
#include <stdarg.h>

#define snprintf c99_snprintf
#define vsnprintf c99_vsnprintf

CMARK_INLINE int c99_vsnprintf(char *outBuf, size_t size, const char *format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(outBuf, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

CMARK_INLINE int c99_snprintf(char *outBuf, size_t size, const char *format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(outBuf, size, format, ap);
    va_end(ap);

    return count;
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* not CMARK_USE_CMAKE_HEADERS */

#endif /* not CMARK_CONFIG_H */
