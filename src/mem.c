#include "cmark-gfm.h"
#include "mem.h"

void *cmark_mem_calloc(cmark_mem *mem, size_t count, size_t size) {
  return mem->calloc(count, size);
}

void *cmark_mem_realloc(cmark_mem *mem, void *ptr, size_t size) {
  return mem->realloc(ptr, size);
}

void cmark_mem_free(cmark_mem *mem, void *ptr) {
  mem->free(ptr);
}

#if defined(_MALLOC_TYPE_ENABLED) && _MALLOC_TYPE_ENABLED

void *cmark_mem_calloc_typed(cmark_mem *mem, size_t count, size_t size, cmark_malloc_type_id type_id) {
  if (mem->calloc_typed)
    return mem->calloc_typed(count, size, type_id);

  return mem->calloc(count, size);
}

void *cmark_mem_realloc_typed(cmark_mem *mem, void *ptr, size_t size, cmark_malloc_type_id type_id) {
  if (mem->realloc_typed)
    return mem->realloc_typed(ptr, size, type_id);

  return mem->realloc(ptr, size);
}

#else

void *cmark_mem_calloc_typed(cmark_mem *mem, size_t count, size_t size, cmark_malloc_type_id type_id) {
  return cmark_mem_calloc(mem, count, size);
}

void *cmark_mem_realloc_typed(cmark_mem *mem, void *ptr, size_t size, cmark_malloc_type_id type_id) {
  return cmark_mem_realloc(mem, ptr, size);
}

#endif
