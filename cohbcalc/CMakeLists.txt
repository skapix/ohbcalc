cmake_minimum_required(VERSION 3.0)

project(cohbcalc)

set(sources ${CMAKE_CURRENT_SOURCE_DIR}/cohbcalc.cpp ConsoleHandler.cpp ConsoleHandler.h KeyBindings.h ExpressionHistory.cpp ExpressionHistory.h)

add_executable(cohbcalc ${sources})

set_property(TARGET cohbcalc PROPERTY CXX_STANDARD 17)
target_link_libraries(cohbcalc ohbcalc ${AdditionalLibraries})

install(TARGETS cohbcalc DESTINATION bin)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/bin/cohbcalc)