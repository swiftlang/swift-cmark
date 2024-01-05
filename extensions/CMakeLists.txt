set(LIBRARY "libcmark-gfm-extensions")

include_directories(include)

add_library(${LIBRARY}
  autolink.c
  core-extensions.c
  ext_scanners.c
  ext_scanners.h
  ext_scanners.re
  strikethrough.c
  table.c
  tagfilter.c
  tasklist.c)
target_compile_definitions(${LIBRARY} PUBLIC
  $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:CMARK_GFM_STATIC_DEFINE>)

set_target_properties(${LIBRARY} PROPERTIES
  OUTPUT_NAME "cmark-gfm-extensions"
  DEFINE_SYMBOL "libcmark_gfm_EXPORTS"
  SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.gfm.${PROJECT_VERSION_GFM}
  VERSION ${PROJECT_VERSION})

set_property(TARGET ${LIBRARY}
  APPEND PROPERTY MACOSX_RPATH true)

# Avoid name clash between PROGRAM and LIBRARY pdb files.
set_target_properties(${LIBRARY} PROPERTIES PDB_NAME cmark-gfm-extensions_dll)

list(APPEND CMARK_INSTALL ${LIBRARY})
target_link_libraries(${LIBRARY} libcmark-gfm)



set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)

install(TARGETS ${CMARK_INSTALL}
  EXPORT cmark-gfm-extensions
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/cmark-gfm-core-extensions.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/module.modulemap
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmark_gfm_extensions)

install(EXPORT cmark-gfm-extensions
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake-gfm-extensions)
