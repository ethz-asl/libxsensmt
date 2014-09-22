cmake_minimum_required(VERSION 2.6)

include(ReMake)

remake_project(
  libxsensmt
  VERSION 4.2
  RELEASE beta
  SUMMARY "Xsens Device API (XDA)."
  AUTHOR "Xsens Technologies BV"
  CONTACT "info@xsens.com"
  AUTHOR "Jerome Maye"
  CONTACT "jerome.maye@mavt.ethz.ch"
  HOME "https://github.com/jmaye/libXsensMT"
  LICENSE "MT SDK Software License Agreement"
  PREFIX XsensMt-
  INSTALL /usr/
  EXTRA_CXX_FLAGS -std=c++0x
)

remake_doc(
  html
  latex
  vendor
)

remake_add_directories(
  lib
)

remake_pkg_config_generate(EXTRA_LIBS
  -L${CMAKE_INSTALL_PREFIX}/${LIBXSENSMT_LIBRARY_DESTINATION} -lxsensdeviceapi
  -lxstypes)

remake_pack_deb(
  DESCRIPTION "Main library"
)

remake_pack_deb(
  COMPONENT dev
  DESCRIPTION "Development headers"
  DEPENDS libxsensmt
)

remake_pack_deb(
  COMPONENT examples
  DESCRIPTION "Examples"
  DEPENDS libxsensmt
)

remake_pack_deb(
  COMPONENT doc
  DESCRIPTION "Documentation"
  DEPENDS libxsensmt
)

remake_distribute_deb(
  DISTRIBUTION lucid
  SECTION libs
  UPLOAD ppa:jmaye/ethz
  DEPENDS remake doxygen pkg-config
  PASS CMAKE_BUILD_TYPE LIBXSENSMT_GIT_REVISION
)

remake_distribute_deb(
  DISTRIBUTION precise
  SECTION libs
  UPLOAD ppa:jmaye/ethz
  DEPENDS remake doxygen pkg-config
  PASS CMAKE_BUILD_TYPE LIBXSENSMT_GIT_REVISION
)

remake_distribute_deb(
  DISTRIBUTION trusty
  SECTION libs
  UPLOAD ppa:jmaye/ethz
  DEPENDS remake doxygen pkg-config
  PASS CMAKE_BUILD_TYPE LIBXSENSMT_GIT_REVISION
)
