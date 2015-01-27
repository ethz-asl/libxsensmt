if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  remake_add_directories(x86_64)
else()
  remake_add_directories(i386)
endif()

remake_pkg_config_generate(EXTRA_LIBS -lxsensdeviceapi -lxstypes)
