cmake_minimum_required(VERSION 3.0)

if (${UNIX})
  find_package(Curses REQUIRED)
  set(AdditionalLibraries ${CURSES_LIBRARIES})
else()
  message(FATAL_ERROR "Windows is currently unsupported")
endif()


project(cHexCalc)

set(sources ${CMAKE_CURRENT_SOURCE_DIR}/chexcalc.cpp ConsoleHandler.cpp ConsoleHandler.h KeyBindings.h)

add_executable(cHexCalc ${sources})

set_property(TARGET cHexCalc PROPERTY CXX_STANDARD 17)

target_link_libraries(cHexCalc HexCalcLib ${AdditionalLibraries})