cmake_minimum_required(VERSION 3.0)

project(ohbcalc)
set(GSL_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY})
set(GSL_TEST_FILE ${GSL_TEST_DIR}/gsl-test.cpp)


#Try find GSL project
set(GSL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GSL/include")
set(INCLUDE_DIRECTORIES "${INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR}/GSL/include")
file(WRITE ${GSL_TEST_FILE}
"#include <gsl/gsl>
#include <gsl/span>
int main() {
	gsl::cstring_span<> S = \"Hello, GSL\";
	return 0;
}
"
)
try_compile(GSL_COMPILED ${GSL_TEST_DIR} SOURCES ${GSL_TEST_FILE}
    COMPILE_DEFINITIONS -I${GSL_INCLUDE_DIR})
#
# Project has a GSL dependency. if OHBCalcLib can not be compiled =>
# we should download project from git repository
if (NOT GSL_COMPILED)
	message(STATUS "GSL was not found in system. Start downloading it.")
  message(STATUS "If something goes wrong, update submodule by yourselves with commands:\
    \n\t\t\tgit init\
    \n\t\t\tgit update")
  find_program(GIT_COMMAND NAMES git)
  if (NOT GIT_COMMAND)
    message(FATAL_ERROR "Could not find git command. Install it and retry")
  endif()
  message(STATUS Git found: ${GIT_COMMAND})

	execute_process(COMMAND "${GIT_COMMAND}" submodule init RESULT_VARIABLE GSL_INITED)
	if (NOT GSL_INITED EQUAL 0)
		message(FATAL_ERROR "Can't initialize submodule GSL in lib directory")
	endif()

	execute_process(COMMAND "${GIT_COMMAND}" submodule update RESULT_VARIABLE GSL_UPDATED)
	if (NOT GSL_UPDATED EQUAL 0)
		message(FATAL_ERROR "Can't update submodule GSL in lib directory")
	endif()

	message(STATUS "GSL was successfully downloaded")
endif(NOT GSL_COMPILED)

set(public_include include/ohbTransform.h include/transformImpl/TransformImpl.h
		include/ohbException.h include/ohbcalc.h)

set(sources source/OHBCalcLib.cpp source/OHBCalcImpl.cpp source/OHBCalcImpl.h
		source/TokenOperation.cpp source/TokenOperation.h source/CommonDefines.h
		source/CommonDefines.cpp ${public_include})

add_library(ohbcalc ${sources})
target_include_directories(ohbcalc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(ohbcalc PRIVATE ${GSL_INCLUDE_DIR})

## Testing
add_subdirectory(test)

install(TARGETS ohbcalc DESTINATION lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)
