cmake_minimum_required(VERSION 3.0)

project(gohbcalc)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)

set(resources resources/resources.qrc)
QT5_ADD_RESOURCES(resources_rcc ${resources})

set(sources gohbcalc.cpp CalculatorWindow.cpp CalculatorWindow.h
    ResultView.h ResultView.cpp)

if (WIN32)
    set(GUI_EXECUTABLE WIN32)
endif()
    
add_executable(gohbcalc ${GUI_EXECUTABLE} ${sources} ${resources_rcc})

set_property(TARGET gohbcalc PROPERTY CXX_STANDARD 14)

target_include_directories(gohbcalc PRIVATE ${CMAKE_SOURCE_DIR}/common)
target_link_libraries(gohbcalc ohbcalc)
target_link_libraries(gohbcalc Qt5::Core Qt5::Widgets)

install(TARGETS gohbcalc DESTINATION bin)