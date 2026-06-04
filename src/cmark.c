#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "registry.h"
#include "node.h"
#include "houdini.h"
#include "cmark-gfm.h"
#include "buffer.h"
#include "mem.h"

#define START_TYPE_ALLOCATOR_IMPL \
  _Pragma("clang diagnostic push") \
  _Pragma("clang diagnostic ignored \"-Wallocator-wrappers\"") \
  _Pragma("clang diagnostic ignored \"-Wunguarded-availability-new\"")
#define END_TYPE_ALLOCATOR_IMPL \
  _Pragma("clang diagnostic pop")

cmark_node_type CMARK_NODE_LAST_BLOCK = CMARK_NODE_FOOTNOTE_DEFINITION;
cmark_node_type CMARK_NODE_LAST_INLINE = CMARK_NODE_ATTRIBUTE;

int cmark_version(void) { return CMARK_GFM_VERSION; }

const char *cmark_version_string(void) { return CMARK_GFM_VERSION_STRING; }

#if defined(_MALLOC_TYPE_ENABLED) && _MALLOC_TYPE_ENABLED

// Native typed-memory allocators are only available on Apple platforms starting at macOS 14+,
// so weak-import them here. These definitions are taken from the back-deploy definitions in
// the macOS system headers
__attribute__((weak_import)) void * __sized_by_or_null(count * size) malloc_type_calloc(size_t count, size_t size, malloc_type_id_t type_id) __result_use_check __alloc_size(1,2);
__attribute__((weak_import)) void * __sized_by_or_null(size) malloc_type_realloc(void * __unsafe_indexable ptr, size_t size, malloc_type_id_t type_id) __result_use_check __alloc_size(2);

// These are the typed allocator versions of the allocator wrappers.
// The typed-operation behavior is managed in mem.c
START_TYPE_ALLOCATOR_IMPL

static void *xcalloc_typed(size_t nmem, size_t size, cmark_malloc_type_id type_id) {
  void *ptr;
  if (malloc_type_calloc)
    ptr = malloc_type_calloc(nmem, size, type_id);
  else {
    #if defined(_MALLOC_TYPE_MALLOC_IS_BACKDEPLOYING) && _MALLOC_TYPE_MALLOC_IS_BACKDEPLOYING
    ptr = malloc_type_calloc_backdeploy(nmem, size, type_id);
    #else
    ptr = calloc(nmem, size);
    #endif
  }
  if (!ptr) {
    fprintf(stderr, "[cmark] calloc returned null pointer, aborting\n");
    abort();
  }
  return ptr;
}

// xcalloc_typed handles the back-deploy logic itself, so treat them the same
#define xcalloc_typed_backdeploy xcalloc_typed

static void *xrealloc_typed(void *ptr, size_t size, cmark_malloc_type_id type_id) {
  void *new_ptr;
  if (malloc_type_realloc) {
    new_ptr = malloc_type_realloc(ptr, size, type_id);
  } else {
    #if defined(_MALLOC_TYPE_MALLOC_IS_BACKDEPLOYING) && _MALLOC_TYPE_MALLOC_IS_BACKDEPLOYING
    new_ptr = malloc_type_realloc_backdeploy(ptr, size, type_id);
    #else
    new_ptr = realloc(ptr, size);
    #endif
  }
  if (!new_ptr) {
    fprintf(stderr, "[cmark] realloc returned null pointer, aborting\n");
    abort();
  }
  return new_ptr;
}

#define xrealloc_typed_backdeploy xrealloc_typed

END_TYPE_ALLOCATOR_IMPL

#else

static void *xcalloc(size_t nmem, size_t size);
static void *xrealloc(void *ptr, size_t size);

static void *xcalloc_typed(size_t nmem, size_t size, cmark_malloc_type_id type_id) {
  return xcalloc(nmem, size);
}

static void *xrealloc_typed(void *ptr, size_t size, cmark_malloc_type_id type_id) {
  return xrealloc(ptr, size);
}

#endif

// When type-aware allocators are available, these functions participate in the
// type-aware allocations machinery. This behavior is managed in mem.c
START_TYPE_ALLOCATOR_IMPL

static void *xcalloc(size_t nmem, size_t size) {
  void *ptr = calloc(nmem, size);
  if (!ptr) {
    fprintf(stderr, "[cmark] calloc returned null pointer, aborting\n");
    abort();
  }
  return ptr;
}

static void *xrealloc(void *ptr, size_t size) {
  void *new_ptr = realloc(ptr, size);
  if (!new_ptr) {
    fprintf(stderr, "[cmark] realloc returned null pointer, aborting\n");
    abort();
  }
  return new_ptr;
}

static void xfree(void *ptr) {
  free(ptr);
}

END_TYPE_ALLOCATOR_IMPL

cmark_mem CMARK_DEFAULT_MEM_ALLOCATOR = {xcalloc, xrealloc, xfree, xcalloc_typed, xrealloc_typed};

cmark_mem *cmark_get_default_mem_allocator(void) {
  return &CMARK_DEFAULT_MEM_ALLOCATOR;
}

char *cmark_markdown_to_html(const char *text, size_t len, int options) {
  cmark_node *doc;
  char *result;

  doc = cmark_parse_document(text, len, options);

  result = cmark_render_html(doc, options, NULL);
  cmark_node_free(doc);

  return result;
}
