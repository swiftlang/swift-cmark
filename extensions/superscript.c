#include "superscript.h"
#include <inlines.h>
#include <parser.h>
#include <render.h>

static cmark_chunk *S_get_node_literal_chunk(cmark_node *node) {
  if (node == NULL) {
    return NULL;
  }

  switch (node->type) {
  case CMARK_NODE_HTML_BLOCK:
  case CMARK_NODE_TEXT:
  case CMARK_NODE_HTML_INLINE:
  case CMARK_NODE_CODE:
  case CMARK_NODE_FOOTNOTE_REFERENCE:
    return &node->as.literal;

  case CMARK_NODE_CODE_BLOCK:
    return &node->as.code.literal;

  default:
    break;
  }

  return NULL;
}

static bool S_node_contains_space(cmark_node *node) {
  cmark_chunk *chunk = S_get_node_literal_chunk(node);
  if (chunk)
    return (cmark_chunk_strchr(chunk, ' ', 0) != chunk->len);
  else
    return false;
}

static bool S_children_contain_space(cmark_node *parent) {
  cmark_node *node = parent->first_child;
  while (node) {
    if (S_node_contains_space(node)) {
      return true;
    }
    node = node->next;
  }

  return false;
}

cmark_node_type CMARK_NODE_SUPERSCRIPT;

static cmark_node *match(cmark_syntax_extension *self, cmark_parser *parser,
                         cmark_node *parent, unsigned char character,
                         cmark_inline_parser *inline_parser) {
  cmark_node *res = NULL;
  int initpos = cmark_inline_parser_get_offset(inline_parser);

  if (character == '^') {
    if (cmark_inline_parser_peek_at(inline_parser, initpos + 1) == '(') {
      res = cmark_node_new_with_mem(CMARK_NODE_TEXT, parser->mem);
      cmark_node_set_literal(res, "^(");
      res->start_line = res->end_line = cmark_inline_parser_get_line(inline_parser);
      res->start_column = cmark_inline_parser_get_column(inline_parser);
      res->end_column = res->start_column + 2;

      cmark_inline_parser_set_offset(inline_parser, initpos + 2);
      cmark_inline_parser_push_delimiter(inline_parser, '^', true, false, res);
    } else {
      int startpos = initpos + 1;
      int endpos = startpos;

      cmark_chunk *chunk = cmark_inline_parser_get_chunk(inline_parser);
      bufsize_t len = chunk->len;

      while (endpos < len) {
        unsigned char seekchar = cmark_inline_parser_peek_at(inline_parser, endpos);
        if (cmark_isspace(seekchar) || (cmark_ispunct(seekchar) && seekchar != '^'))
          break;
        endpos++;
      }

      int nodelen = endpos - startpos;

      // don't emit an empty node
      if (nodelen == 0)
        return NULL;

      cmark_inline_parser_set_offset(inline_parser, startpos);

      res = cmark_node_new_with_mem_and_ext(CMARK_NODE_SUPERSCRIPT, parser->mem, self);
      res->as.literal = cmark_chunk_dup(chunk, startpos, nodelen);
      res->start_line = cmark_inline_parser_get_line(inline_parser);
      res->start_column = cmark_inline_parser_get_column(inline_parser);

      cmark_inline_parser_set_offset(inline_parser, endpos);

      res->end_line = cmark_inline_parser_get_line(inline_parser);
      res->end_column = cmark_inline_parser_get_column(inline_parser);

      const char *text = cmark_chunk_to_cstr(parser->mem, &res->as.literal);
      cmark_node_set_string_content(res, text);

      cmark_parse_inlines(parser, res, parser->refmap, parser->options);
    }
  } else if (character == ')') {
    res = cmark_node_new_with_mem(CMARK_NODE_TEXT, parser->mem);
    cmark_node_set_literal(res, ")");
    res->start_line = res->end_line = cmark_inline_parser_get_line(inline_parser);
    res->start_column = cmark_inline_parser_get_column(inline_parser);
    res->end_column = res->start_column + 1;

    cmark_inline_parser_set_offset(inline_parser, initpos + 1);
    cmark_inline_parser_push_delimiter(inline_parser, '^', false, true, res);
  }

  return res;
}

static delimiter *insert(cmark_syntax_extension *self, cmark_parser *parser,
                         cmark_inline_parser *inline_parser, delimiter *opener,
                         delimiter *closer) {
  cmark_node *superscript;
  cmark_node *tmp, *next;
  delimiter *delim, *tmp_delim;
  delimiter *res = closer->next;

  superscript = opener->inl_text;

  if (!cmark_node_set_type(superscript, CMARK_NODE_SUPERSCRIPT))
    return res;

  cmark_node_set_syntax_extension(superscript, self);

  tmp = cmark_node_next(opener->inl_text);

  while (tmp) {
    if (tmp == closer->inl_text)
      break;
    next = cmark_node_next(tmp);
    cmark_node_append_child(superscript, tmp);
    tmp = next;
  }

  superscript->end_column = closer->inl_text->start_column + closer->inl_text->as.literal.len - 1;
  cmark_node_free(closer->inl_text);

  delim = closer;
  while (delim != NULL && delim != opener) {
    tmp_delim = delim->previous;
    cmark_inline_parser_remove_delimiter(inline_parser, delim);
    delim = tmp_delim;
  }

  cmark_inline_parser_remove_delimiter(inline_parser, opener);

  return res;
}

static const char *get_type_string(cmark_syntax_extension *extension,
                                   cmark_node *node) {
  return node->type == CMARK_NODE_SUPERSCRIPT ? "superscript" : "<unknown>";
}

static int can_contain(cmark_syntax_extension *extension, cmark_node *node,
                       cmark_node_type child_type) {
  if (node->type != CMARK_NODE_SUPERSCRIPT)
    return false;

  return CMARK_NODE_TYPE_INLINE_P(child_type);
}

static void commonmark_render(cmark_syntax_extension *extension,
                              cmark_renderer *renderer, cmark_node *node,
                              cmark_event_type ev_type, int options) {
  bool should_wrap = S_children_contain_space(node);
  bool entering = (ev_type == CMARK_EVENT_ENTER);
  if (entering) {
    if (should_wrap)
      renderer->out(renderer, node, "^(", false, LITERAL);
    else
      renderer->out(renderer, node, "^", false, LITERAL);
  } else if (!entering && should_wrap) {
    renderer->out(renderer, node, ")", false, LITERAL);
  }
}

static void latex_render(cmark_syntax_extension *extension,
                         cmark_renderer *renderer, cmark_node *node,
                         cmark_event_type ev_type, int options) {
  bool entering = (ev_type == CMARK_EVENT_ENTER);
  if (entering) {
    renderer->out(renderer, node, "^{", false, LITERAL);
  } else {
    renderer->out(renderer, node, "}", false, LITERAL);
  }
}

static void man_render(cmark_syntax_extension *extension,
                       cmark_renderer *renderer, cmark_node *node,
                       cmark_event_type ev_type, int options) {
  // requires MOM
  bool entering = (ev_type == CMARK_EVENT_ENTER);
  if (entering) {
    renderer->cr(renderer);
    renderer->out(renderer, node, "\\*[SUP]", false, LITERAL);
  } else {
    renderer->out(renderer, node, "\\*[SUPX]", false, LITERAL);
    renderer->cr(renderer);
  }
}

static void html_render(cmark_syntax_extension *extension,
                        cmark_html_renderer *renderer, cmark_node *node,
                        cmark_event_type ev_type, int options) {
  bool entering = (ev_type == CMARK_EVENT_ENTER);
  if (entering) {
    cmark_strbuf_puts(renderer->html, "<sup>");
  } else {
    cmark_strbuf_puts(renderer->html, "</sup>");
  }
}

cmark_syntax_extension *create_superscript_extension(void) {
  cmark_syntax_extension *ext = cmark_syntax_extension_new("superscript");
  cmark_llist *special_chars = NULL;

  cmark_syntax_extension_set_get_type_string_func(ext, get_type_string);
  cmark_syntax_extension_set_can_contain_func(ext, can_contain);
  cmark_syntax_extension_set_commonmark_render_func(ext, commonmark_render);
  cmark_syntax_extension_set_plaintext_render_func(ext, commonmark_render);
  cmark_syntax_extension_set_latex_render_func(ext, latex_render);
  cmark_syntax_extension_set_man_render_func(ext, man_render);
  cmark_syntax_extension_set_html_render_func(ext, html_render);
  CMARK_NODE_SUPERSCRIPT = cmark_syntax_extension_add_node(1);

  cmark_syntax_extension_set_match_inline_func(ext, match);
  cmark_syntax_extension_set_inline_from_delim_func(ext, insert);

  cmark_mem *mem = cmark_get_default_mem_allocator();
  special_chars = cmark_llist_append(mem, special_chars, (void *)'^');
  special_chars = cmark_llist_append(mem, special_chars, (void *)')');
  cmark_syntax_extension_set_special_inline_chars(ext, special_chars);

  cmark_syntax_extension_set_emphasis(ext, 1);

  return ext;
}
