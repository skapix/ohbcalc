cmake_minimum_required(VERSION 3.0)

project(ohbcalc)

set(public_include include/ohbTransform.h include/transformImpl/TransformImpl.h
                include/ohbException.h include/ohbcalc.h)

set(sources source/OHBCalcLib.cpp source/OHBCalcImpl.cpp source/OHBCalcImpl.h
		source/TokenOperation.cpp source/TokenOperation.h source/CommonDefines.h
		source/CommonDefines.cpp ${public_include})

add_library(ohbcalc ${sources})
target_include_directories(ohbcalc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

## Testing
add_subdirectory(test)

install(TARGETS ohbcalc DESTINATION lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)
