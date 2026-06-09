# CMake generated Testfile for 
# Source directory: C:/dev/qt-ci-demo
# Build directory: C:/dev/qt-ci-demo/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test([=[UnitTests]=] "C:/dev/qt-ci-demo/build/Debug/UnitTests.exe")
  set_tests_properties([=[UnitTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/dev/qt-ci-demo/CMakeLists.txt;34;add_test;C:/dev/qt-ci-demo/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test([=[UnitTests]=] "C:/dev/qt-ci-demo/build/Release/UnitTests.exe")
  set_tests_properties([=[UnitTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/dev/qt-ci-demo/CMakeLists.txt;34;add_test;C:/dev/qt-ci-demo/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test([=[UnitTests]=] "C:/dev/qt-ci-demo/build/MinSizeRel/UnitTests.exe")
  set_tests_properties([=[UnitTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/dev/qt-ci-demo/CMakeLists.txt;34;add_test;C:/dev/qt-ci-demo/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test([=[UnitTests]=] "C:/dev/qt-ci-demo/build/RelWithDebInfo/UnitTests.exe")
  set_tests_properties([=[UnitTests]=] PROPERTIES  _BACKTRACE_TRIPLES "C:/dev/qt-ci-demo/CMakeLists.txt;34;add_test;C:/dev/qt-ci-demo/CMakeLists.txt;0;")
else()
  add_test([=[UnitTests]=] NOT_AVAILABLE)
endif()
