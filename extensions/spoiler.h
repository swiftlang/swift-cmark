#ifndef CMARK_GFM_SPOILER_H
#define CMARK_GFM_SPOILER_H

#include "cmark-gfm-core-extensions.h"

extern cmark_node_type CMARK_NODE_SPOILER;
cmark_syntax_extension *create_spoiler_extension(void);

#endif /* CMARK_GFM_SPOILER_H */
