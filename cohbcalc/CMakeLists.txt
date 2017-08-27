cmake_minimum_required(VERSION 3.8)

project(cohbcalc)

# cxx_std_17 does not work with MSVC. Do it manually.
if (MSVC)
  if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" cpp_latest_flag_supported)
    if (cpp_latest_flag_supported)
      add_compile_options("/std:c++latest")
    endif()
  else()
    message(WARNING "Project requires c++17. Skip cohbcalc")
    return()
  endif()
endif(MSVC)

set(sources cohbcalc.cpp ConsoleHandler.cpp ConsoleHandler.h KeyHandler.h
    ExpressionHistory.cpp ExpressionHistory.h OutputRepresentation.cpp OutputRepresentation.h)

if (WIN32)
    set(sources ${sources} WindowsKeyHandler.cpp)
elseif (UNIX AND NOT APPLE)
    set(sources ${sources} LinuxKeyHandler.cpp)
else()
    message(WARNING "cohbcalc does not support this system. Project cohbcalc is disabled")
    return()
endif()
    
add_executable(cohbcalc ${sources})

set_property(TARGET cohbcalc PROPERTY CXX_STANDARD 17)

target_link_libraries(cohbcalc ohbcalc)

install(TARGETS cohbcalc DESTINATION bin)
