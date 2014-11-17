cmake_minimum_required(VERSION 2.6)

include(ReMake)

remake_project(
  libxsensmt
  VERSION 4.2
  RELEASE beta
  SUMMARY "Software development kit for Xsens motion tracking (MT) devices."
  AUTHOR "Xsens Technologies BV"
  CONTACT "info@xsens.com"
  HOME "https://www.xsens.com/mt-software-suite"
  LICENSE "MT SDK Software License Agreement"
  PREFIX xsensmt-
  INSTALL /usr
)

remake_doc(
  html
  vendor
)

remake_add_directories(lib)

remake_project_get(LIBRARY_DESTINATION DESTINATION)
remake_pkg_config_generate(EXTRA_LIBS
  -L${LIBRARY_DESTINATION} -lxsensdeviceapi -lxstypes)

remake_pack_deb(
  DEPENDS liblapack3g?f?
)

remake_pack_deb(
  COMPONENT dev
  DESCRIPTION "development headers"
  DEPENDS libxsensmt
)

remake_pack_deb(
  COMPONENT examples
  DESCRIPTION "examples"
  DEPENDS libxsensmt
)

remake_pack_deb(
  COMPONENT doc
  DESCRIPTION "documentation"
  DEPENDS libxsensmt
)

remake_distribute_deb(
  DISTRIBUTION precise
  SECTION libs
  UPLOAD ppa:ethz-asl/common
  DEPENDS remake doxygen pkg-config liblapack-dev
  PASS CMAKE_BUILD_TYPE LIBXSENSMT_GIT_REVISION
)

remake_distribute_deb(
  DISTRIBUTION trusty
  SECTION libs
  UPLOAD ppa:ethz-asl/common
  DEPENDS remake doxygen pkg-config liblapack-dev
  PASS CMAKE_BUILD_TYPE LIBXSENSMT_GIT_REVISION
)
