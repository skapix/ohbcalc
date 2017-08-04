cmake_minimum_required(VERSION 3.0)

if (${UNIX})
  find_package(Curses REQUIRED)
  set(AdditionalLibraries ${CURSES_LIBRARIES})
else()
  message(FATAL_ERROR "Windows is currently unsupported")
endif()


project(cohbcalc)

set(sources ${CMAKE_CURRENT_SOURCE_DIR}/cohbcalc.cpp ConsoleHandler.cpp ConsoleHandler.h KeyBindings.h ExpressionHistory.cpp ExpressionHistory.h)

add_executable(cohbcalc ${sources})

set_property(TARGET cohbcalc PROPERTY CXX_STANDARD 17)
target_link_libraries(cohbcalc ohbcalc ${AdditionalLibraries})