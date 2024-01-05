set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 29)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_GFM 13)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmark-gfm_version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmark-gfm_version.h)

check_file_offset_bits()

add_executable(cmark-gfm
  ${PROJECT_SOURCE_DIR}/bin/main.c)
target_link_libraries(cmark-gfm
  libcmark-gfm
  libcmark-gfm-extensions)

add_library(libcmark-gfm
  arena.c
  blocks.c
  buffer.c
  cmark.c
  cmark_ctype.c
  commonmark.c
  footnotes.c
  houdini_href_e.c
  houdini_html_e.c
  houdini_html_u.c
  html.c
  inlines.c
  iterator.c
  latex.c
  linked_list.c
  man.c
  map.c
  node.c
  plaintext.c
  plugin.c
  references.c
  registry.c
  render.c
  scanners.c
  scanners.re
  syntax_extension.c
  utf8.c
  xml.c)
target_compile_definitions(libcmark-gfm PUBLIC
  $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:CMARK_GFM_STATIC_DEFINE>)
target_include_directories(libcmark-gfm PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/extensions/include>)
  target_link_libraries(libcmark-gfm PRIVATE
    $<$<BOOL:${THREADS_FOUND}>:Threads::Threads>)
set_target_properties(libcmark-gfm PROPERTIES
  MACOSX_RPATH TRUE
  OUTPUT_NAME cmark-gfm
  PDB_NAME libcmark-gfm
  SOVERSION ${PROJECT_VERSION}
  VERSION ${PROJECT_VERSION})

list(APPEND CMARK_INSTALL libcmark-gfm)

install(TARGETS cmark-gfm ${CMARK_INSTALL}
  EXPORT cmark-gfm
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libcmark-gfm.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libcmark-gfm.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcmark-gfm.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/chunk.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/cmark_ctype.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/cmark-gfm.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/cmark-gfm_config.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/cmark-gfm-extension_api.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/cmark-gfm_version.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/export.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/footnotes.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/houdini.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/html.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/inlines.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/iterator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/map.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/mutex.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/node.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/parser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/plugin.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/references.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/registry.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/render.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/scanners.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/syntax_extension.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/utf8.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/module.modulemap
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmark_gfm
  )

install(EXPORT cmark-gfm
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

export(TARGETS ${CMARK_INSTALL}
  FILE ${CMAKE_CURRENT_BINARY_DIR}/cmarkTargets.cmake)

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)
