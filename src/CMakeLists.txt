set(LIBRARY "libcmark-gfm")
set(STATICLIBRARY "libcmark-gfm_static")
set(LIBRARY_SOURCES
  cmark.c
  node.c
  iterator.c
  blocks.c
  inlines.c
  scanners.c
  scanners.re
  utf8.c
  buffer.c
  references.c
  footnotes.c
  map.c
  render.c
  man.c
  xml.c
  html.c
  commonmark.c
  plaintext.c
  latex.c
  houdini_href_e.c
  houdini_html_e.c
  houdini_html_u.c
  cmark_ctype.c
  arena.c
  linked_list.c
  syntax_extension.c
  registry.c
  plugin.c
  )

set(PROGRAM "cmark-gfm")

include_directories(include ${CMAKE_CURRENT_BINARY_DIR})
include_directories(
  ${PROJECT_SOURCE_DIR}/extensions/include
  ${PROJECT_BINARY_DIR}/extensions
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmark-gfm_version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmark-gfm_version.h)

check_file_offset_bits()

add_executable(${PROGRAM}
  ${PROJECT_SOURCE_DIR}/bin/main.c)

if(CMARK_SHARED)
  target_link_libraries(${PROGRAM} libcmark-gfm-extensions libcmark-gfm)
elseif(CMARK_STATIC)
  target_link_libraries(${PROGRAM} libcmark-gfm-extensions_static libcmark-gfm_static)
endif()

if (CMARK_SHARED)
  add_library(${LIBRARY} SHARED ${LIBRARY_SOURCES})
  target_include_directories(${LIBRARY} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/extensions/include>)
    target_link_libraries(${LIBRARY} PRIVATE
      $<$<BOOL:${THREADS_FOUND}>:Threads::Threads>)

  # Include minor version and patch level in soname for now.
  set_target_properties(${LIBRARY} PROPERTIES
    OUTPUT_NAME "cmark-gfm"
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.gfm.${PROJECT_VERSION_GFM}
    VERSION ${PROJECT_VERSION})

  set_property(TARGET ${LIBRARY}
    APPEND PROPERTY MACOSX_RPATH true)

  # Avoid name clash between PROGRAM and LIBRARY pdb files.
  set_target_properties(${LIBRARY} PROPERTIES PDB_NAME cmark-gfm_dll)

  list(APPEND CMARK_INSTALL ${LIBRARY})
endif()

if (CMARK_STATIC)
  add_library(${STATICLIBRARY} STATIC ${LIBRARY_SOURCES})
  target_compile_definitions(${STATICLIBRARY} PUBLIC
    CMARK_GFM_STATIC_DEFINE)
  target_include_directories(${LIBRARY} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/extensions/include>)
  target_link_libraries(${STATICLIBRARY} PRIVATE
    $<$<BOOL:${Threads_FOUND}>:Threads::Threads>)

  set_target_properties(${STATICLIBRARY} PROPERTIES
    POSITION_INDEPENDENT_CODE ON)

  if (MSVC)
    set_target_properties(${STATICLIBRARY} PROPERTIES
      OUTPUT_NAME "cmark-gfm_static"
      VERSION ${PROJECT_VERSION})
  else()
    set_target_properties(${STATICLIBRARY} PROPERTIES
      OUTPUT_NAME "cmark-gfm"
      VERSION ${PROJECT_VERSION})
  endif(MSVC)

  list(APPEND CMARK_INSTALL ${STATICLIBRARY})
endif()

install(TARGETS ${PROGRAM} ${CMARK_INSTALL}
  EXPORT cmark-gfm
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

if(CMARK_SHARED OR CMARK_STATIC)
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
endif()

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)
