set(sources
epple2.a2bare.conf.in
epple2.a2dos31.conf.in
epple2.a2dos33.conf.in
epple2.a2loaded.conf.in
epple2.a2pbare.conf.in
epple2.a2pdos33.conf.in
epple2.a2ploaded.conf.in
epple2.conf.in
epple2.rev0bare.conf.in
)

set(LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

foreach(source ${sources})
  cmake_path(GET source STEM LAST_ONLY target)
  configure_file(${source} ${target})
endforeach()
