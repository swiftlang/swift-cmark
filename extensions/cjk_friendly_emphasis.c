#include <stdbool.h>

#include "cjk_friendly_emphasis.h"
#include <parser.h>
#include <render.h>

cmark_syntax_extension *create_cjk_friendly_emphasis_extension(void) {
  cmark_syntax_extension *ext = cmark_syntax_extension_new("cjk_friendly_emphasis");

  cmark_syntax_extension_set_cjk_friendly_emphasis(ext, true);

  return ext;
}
