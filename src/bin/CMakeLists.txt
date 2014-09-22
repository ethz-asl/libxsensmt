remake_include(../lib/)
remake_include(../lib/examples)

remake_add_executables(LINK examples ${XSENSDEVICEAPI_LIBRARY}
  ${XSTYPES_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
