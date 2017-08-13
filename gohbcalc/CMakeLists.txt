cmake_minimum_required(VERSION 3.0)

project(cohbcalc)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)

set(sources gohbcalc.cpp CalculatorWindow.cpp CalculatorWindow.h
    ${CMAKE_SOURCE_DIR}/common/ExpressionHistory.cpp ResultView.h ResultView.cpp)

add_executable(gohbcalc ${sources})

set_property(TARGET cohbcalc PROPERTY CXX_STANDARD 14)

target_include_directories(gohbcalc PRIVATE ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(gohbcalc ohbcalc)
target_link_libraries(gohbcalc Qt5::Core Qt5::Widgets)
