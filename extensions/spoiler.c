#include "spoiler.h"
#include <parser.h>
#include <render.h>

cmark_node_type CMARK_NODE_SPOILER;

static cmark_node *match(cmark_syntax_extension *self, cmark_parser *parser,
                         cmark_node *parent, unsigned char character,
                         cmark_inline_parser *inline_parser) {
  cmark_node *res = NULL;
  int left_flanking, right_flanking, punct_before, punct_after, delims;
  char buffer[101];

  if ((parser->options & CMARK_OPT_SPOILER_REDDIT_STYLE)) {
    // Reddit-style spoilers - flanked by angle brackets and exclamation marks,
    // e.g. >!this is a spoiler!<
    int pos = cmark_inline_parser_get_offset(inline_parser);
    char *txt = NULL;
    bool opener = false;
    bool closer = false;
    if (cmark_inline_parser_peek_at(inline_parser, pos) == '>' &&
        cmark_inline_parser_peek_at(inline_parser, pos + 1) == '!') {
      txt = ">!";
      opener = true;
    } else if (cmark_inline_parser_peek_at(inline_parser, pos) == '!' &&
               cmark_inline_parser_peek_at(inline_parser, pos + 1) == '<') {
      txt = "!<";
      closer = true;
    }

    if (opener && pos > 0 && !cmark_isspace(cmark_inline_parser_peek_at(inline_parser, pos - 1))) {
      opener = false;
    }

    if (closer) {
      cmark_chunk *chunk = cmark_inline_parser_get_chunk(inline_parser);
      bufsize_t len = chunk->len;
      if (pos + 2 < len && !cmark_isspace(cmark_inline_parser_peek_at(inline_parser, pos + 2))) {
        closer = false;
      }
    }

    if ((!opener && !closer) || !txt)
      return NULL;

    res = cmark_node_new_with_mem(CMARK_NODE_TEXT, parser->mem);
    cmark_node_set_literal(res, txt);
    res->start_line = cmark_inline_parser_get_line(inline_parser);
    res->start_column = cmark_inline_parser_get_column(inline_parser);

    cmark_inline_parser_set_offset(inline_parser, pos + 2);

    res->end_line = cmark_inline_parser_get_line(inline_parser);
    res->end_column = cmark_inline_parser_get_column(inline_parser);

    // Set the character for this delimiter to `!`, since it's a heterogenous
    // delimiter and the delimiter API assumes single repeated characters.
    cmark_inline_parser_push_delimiter(inline_parser, '!', opener, closer, res);
  } else {
    // Discord-style spoilers - flanked on both sides by two pipes,
    // e.g. ||this is a spoiler||
    if (character != '|')
      return NULL;

    delims = cmark_inline_parser_scan_delimiters(
        inline_parser, sizeof(buffer) - 1, '|',
        &left_flanking,
        &right_flanking, &punct_before, &punct_after);

    memset(buffer, '|', delims);
    buffer[delims] = 0;

    res = cmark_node_new_with_mem(CMARK_NODE_TEXT, parser->mem);
    cmark_node_set_literal(res, buffer);
    res->start_line = res->end_line = cmark_inline_parser_get_line(inline_parser);
    res->start_column = cmark_inline_parser_get_column(inline_parser) - delims;

    if ((left_flanking || right_flanking) && (delims == 2)) {
      cmark_inline_parser_push_delimiter(inline_parser, character, left_flanking,
                                         right_flanking, res);
    }
  }

  return res;
}

static delimiter *insert(cmark_syntax_extension *self, cmark_parser *parser,
                         cmark_inline_parser *inline_parser, delimiter *opener,
                         delimiter *closer) {
  cmark_node *spoiler;
  cmark_node *tmp, *next;
  delimiter *delim, *tmp_delim;
  delimiter *res = closer->next;

  spoiler = opener->inl_text;

  if (opener->inl_text->as.literal.len != closer->inl_text->as.literal.len)
    goto done;

  if (!cmark_node_set_type(spoiler, CMARK_NODE_SPOILER))
    goto done;

  cmark_node_set_syntax_extension(spoiler, self);

  tmp = cmark_node_next(opener->inl_text);

  while (tmp) {
    if (tmp == closer->inl_text)
      break;
    next = cmark_node_next(tmp);
    cmark_node_append_child(spoiler, tmp);
    tmp = next;
  }

  spoiler->end_column = closer->inl_text->start_column + closer->inl_text->as.literal.len - 1;
  cmark_node_free(closer->inl_text);

  delim = closer;
  while (delim != NULL && delim != opener) {
    tmp_delim = delim->previous;
    cmark_inline_parser_remove_delimiter(inline_parser, delim);
    delim = tmp_delim;
  }

  cmark_inline_parser_remove_delimiter(inline_parser, opener);

done:
  return res;
}

static const char *get_type_string(cmark_syntax_extension *extension,
                                   cmark_node *node) {
  return node->type == CMARK_NODE_SPOILER ? "spoiler" : "<unknown>";
}

static int can_contain(cmark_syntax_extension *extension, cmark_node *node,
                       cmark_node_type child_type) {
  if (node->type != CMARK_NODE_SPOILER)
    return false;

  return CMARK_NODE_TYPE_INLINE_P(child_type);
}

static void commonmark_render(cmark_syntax_extension *extension,
                              cmark_renderer *renderer, cmark_node *node,
                              cmark_event_type ev_type, int options) {
  if (options & CMARK_OPT_SPOILER_REDDIT_STYLE) {
    bool entering = (ev_type == CMARK_EVENT_ENTER);
    if (entering) {
      renderer->out(renderer, node, ">!", false, LITERAL);
    } else {
      renderer->out(renderer, node, "!<", false, LITERAL);
    }
  } else {
    renderer->out(renderer, node, "||", false, LITERAL);
  }
}

static void html_render(cmark_syntax_extension *extension,
                        cmark_html_renderer *renderer, cmark_node *node,
                        cmark_event_type ev_type, int options) {
  bool entering = (ev_type == CMARK_EVENT_ENTER);
  if (entering) {
    cmark_strbuf_puts(renderer->html, "<span class=\"spoiler\">");
  } else {
    cmark_strbuf_puts(renderer->html, "</span>");
  }
}

cmark_syntax_extension *create_spoiler_extension(void) {
  cmark_syntax_extension *ext = cmark_syntax_extension_new("spoiler");
  cmark_llist *special_chars = NULL;

  cmark_syntax_extension_set_get_type_string_func(ext, get_type_string);
  cmark_syntax_extension_set_can_contain_func(ext, can_contain);
  cmark_syntax_extension_set_commonmark_render_func(ext, commonmark_render);
  cmark_syntax_extension_set_html_render_func(ext, html_render);
  cmark_syntax_extension_set_plaintext_render_func(ext, commonmark_render);
  CMARK_NODE_SPOILER = cmark_syntax_extension_add_node(1);

  cmark_syntax_extension_set_match_inline_func(ext, match);
  cmark_syntax_extension_set_inline_from_delim_func(ext, insert);

  cmark_mem *mem = cmark_get_default_mem_allocator();
  special_chars = cmark_llist_append(mem, special_chars, (void *)'|');
  special_chars = cmark_llist_append(mem, special_chars, (void *)'>');
  special_chars = cmark_llist_append(mem, special_chars, (void *)'<');
  special_chars = cmark_llist_append(mem, special_chars, (void *)'!');
  cmark_syntax_extension_set_special_inline_chars(ext, special_chars);

  cmark_syntax_extension_set_emphasis(ext, 1);

  return ext;
}
