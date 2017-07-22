find_package(GTest)
if (NOT ${GTest_FOUND})
  message(WARNING "Can not find GTEST package. Tests are disabled")
  return()
endif(NOT ${GTest_FOUND})

enable_testing()

set(test_source ${CMAKE_CURRENT_SOURCE_DIR}/testHexCalcLib.cpp)
add_executable(HexCalcTest ${test_source})
target_link_libraries(HexCalcTest GTest::GTest GTest::Main HexCalcLib)

add_test(HexCalcTesting HexCalcTest)
