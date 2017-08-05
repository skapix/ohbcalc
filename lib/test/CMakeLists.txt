find_package(GTest)
if (NOT ${GTest_FOUND})
  message(WARNING "Can not find GTEST package. Tests are disabled")
  return()
endif(NOT ${GTest_FOUND})

set(test_source ${CMAKE_CURRENT_SOURCE_DIR}/testOHBcalc.cpp testTransforms.cpp testPosition.cpp)
add_executable(ohbcalcTest ${test_source})
target_link_libraries(ohbcalcTest GTest::GTest GTest::Main ohbcalc)

add_test(NAME ohbcalcTesting COMMAND ohbcalcTest)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS ohbcalcTest)