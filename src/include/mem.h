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

CMARK_GFM_EXPORT
void *cmark_mem_calloc_typed(cmark_mem *mem, size_t count, size_t size, cmark_malloc_type_id type_id);

#define cmark_mem_calloc_typed_backdeploy cmark_mem_calloc_typed

CMARK_GFM_EXPORT
void *cmark_mem_realloc_typed(cmark_mem *mem, void *ptr, size_t size, cmark_malloc_type_id type_id);

#define cmark_mem_realloc_typed_backdeploy cmark_mem_realloc_typed

CMARK_GFM_EXPORT
void *cmark_mem_calloc(cmark_mem *mem, size_t count, size_t size) CMARK_MALLOC_TYPED(cmark_mem_calloc_typed, 3);

#define CMARK_CALLOC(MEM, TYPE, COUNT) (TYPE *)cmark_mem_calloc(MEM, COUNT, sizeof(TYPE))
#define CMARK_MALLOC(MEM, TYPE) CMARK_CALLOC(MEM, TYPE, 1)

CMARK_GFM_EXPORT
void *cmark_mem_realloc(cmark_mem *mem, void *ptr, size_t size) CMARK_MALLOC_TYPED(cmark_mem_realloc_typed, 3);

#define CMARK_REALLOC(MEM, PTR, TYPE, COUNT) (TYPE *)cmark_mem_realloc(MEM, PTR, (COUNT) * sizeof(TYPE))

CMARK_GFM_EXPORT
void cmark_mem_free(cmark_mem *mem, void *ptr);

#ifdef __cplusplus
}
#endif

#endif
