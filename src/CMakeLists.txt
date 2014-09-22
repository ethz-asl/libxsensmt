if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  remake_find_library(xsensdeviceapi xsensdeviceapi.h
    PATHS ${CMAKE_SOURCE_DIR}/lib/x86_64 ${CMAKE_SOURCE_DIR}/src/lib)
  remake_find_library(xstypes xstypes.h
    PATHS ${CMAKE_SOURCE_DIR}/lib/x86_64 ${CMAKE_SOURCE_DIR}/src/lib)
else()
  remake_find_library(xsensdeviceapi xsensdeviceapi.h
    PATHS ${CMAKE_SOURCE_DIR}/lib/i386 ${CMAKE_SOURCE_DIR}/src/lib)
  remake_find_library(xstypes xstypes.h
    PATHS ${CMAKE_SOURCE_DIR}/lib/i386 ${CMAKE_SOURCE_DIR}/src/lib)
endif()
remake_find_package(Threads)
if(NOT ${CMAKE_USE_PTHREADS_INIT})
  message(FATAL_ERROR "Missing POSIX thread support!")
endif(NOT ${CMAKE_USE_PTHREADS_INIT})

remake_add_directories(lib)
remake_add_directories(bin COMPONENT examples)
