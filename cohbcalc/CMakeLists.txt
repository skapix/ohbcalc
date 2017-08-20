cmake_minimum_required(VERSION 3.8)

project(cohbcalc)

set(sources cohbcalc.cpp ConsoleHandler.cpp ConsoleHandler.h KeyHandler.h
    ../common/ExpressionHistory.cpp ../common/ExpressionHistory.h)

if (WIN32)
    set(sources ${sources} WindowsKeyHandler.cpp)
elseif (UNIX AND NOT APPLE)
    set(sources ${sources} LinuxKeyHandler.cpp)
else()
    message(WARNING "cohbcalc does not support this system. Project cohbcalc is disabled")
    return()
endif()
    
add_executable(cohbcalc ${sources})

target_compile_features(cohbcalc PRIVATE cxx_std_17)

target_link_libraries(cohbcalc ohbcalc)

install(TARGETS cohbcalc DESTINATION bin)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_INSTALL_PREFIX}/bin/cohbcalc)