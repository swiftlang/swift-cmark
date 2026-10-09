#include <stdbool.h>

#include "cmark-gfm.h"
#include "parser.h"
#include "references.h"
#include "inlines.h"
#include "chunk.h"
#include "mem.h"

static void reference_free(cmark_map *map, cmark_map_entry *_ref) {
  cmark_reference *ref = (cmark_reference *)_ref;
  cmark_mem *mem = map->mem;
  if (ref != NULL) {
    cmark_mem_free(mem, ref->entry.label);
    cmark_chunk_free(mem, &ref->url);
    cmark_chunk_free(mem, &ref->title);
    cmark_chunk_free(mem, &ref->attributes);
    cmark_mem_free(mem, ref);
  }
}

void cmark_reference_create(cmark_map *map, cmark_chunk *label,
                            cmark_chunk *url, cmark_chunk *title) {
  cmark_reference *ref;
  unsigned char *reflabel = normalize_map_label(map->mem, label);

  /* empty reference name, or composed from only whitespace */
  if (reflabel == NULL)
    return;

  assert(map->sorted == NULL);

  ref = CMARK_CALLOC_ONE(map->mem, cmark_reference);
  ref->entry.label = reflabel;
  ref->is_attributes_reference = false;
  ref->url = cmark_clean_url(map->mem, url);
  ref->title = cmark_clean_title(map->mem, title);
  ref->attributes = cmark_chunk_literal("");
  ref->entry.age = map->size;
  ref->entry.next = map->refs;
  ref->entry.size = ref->url.len + ref->title.len;

  map->refs = (cmark_map_entry *)ref;
  map->size++;
}

void cmark_reference_create_attributes(cmark_map *map, cmark_chunk *label,
                                       cmark_chunk *attributes) {
  cmark_reference *ref;
  unsigned char *reflabel = normalize_map_label(map->mem, label);

  /* empty reference name, or composed from only whitespace */
  if (reflabel == NULL)
    return;

  assert(map->sorted == NULL);

  ref = CMARK_CALLOC_ONE(map->mem, cmark_reference);
  ref->entry.label = reflabel;
  ref->is_attributes_reference = true;
  ref->url = cmark_chunk_literal("");
  ref->title = cmark_chunk_literal("");
  ref->attributes = cmark_clean_attributes(map->mem, attributes);
  ref->entry.age = map->size;
  ref->entry.next = map->refs;
  
  map->refs = (cmark_map_entry *)ref;
  map->size++;
}

cmark_map *cmark_reference_map_new(cmark_mem *mem) {
  return cmark_map_new(mem, reference_free);
}
