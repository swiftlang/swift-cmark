#ifndef CMARK_MEM_H
#define CMARK_MEM_H

#include <stdlib.h>
#include "cmark-gfm.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MALLOC_TYPE_ENABLED) && _MALLOC_TYPE_ENABLED
# define CMARK_MALLOC_TYPED(F,N) _MALLOC_TYPED(F,N)
#else
# define CMARK_MALLOC_TYPED(F,N)
#endif

#ifdef __cplusplus
}
#endif

#endif
