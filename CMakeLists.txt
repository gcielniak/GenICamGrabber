cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(GenICamGrab)

set (EXTRA_INCLUDE_DIRS include)
set (EXTRA_LIBRARY_DIRS "")
set (EXTRA_LIBRARIES "")

#Boost
find_package(BOOST REQUIRED)
set (EXTRA_INCLUDE_DIRS ${EXTRA_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})
set (EXTRA_LIBRARY_DIRS ${EXTRA_LIBRARY_DIRS} $ENV{BOOST_LIBRARYDIR})
set (EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ${Boost_LIBRARIES})
add_definitions(-D_SCL_SECURE_NO_WARNINGS)

# OpenCV
find_package(OpenCV REQUIRED)
set (EXTRA_INCLUDE_DIRS ${EXTRA_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
set (EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ${OpenCV_LIBS})

# Stemmer Library: GenICam driver
if(EXISTS "$ENV{CVB}/Lib/C")
    set(EXTRA_INCLUDE_DIRS ${EXTRA_INCLUDE_DIRS} "$ENV{CVB}/Lib/C")
    set(EXTRA_LIBRARY_DIRS ${EXTRA_LIBRARY_DIRS} "$ENV{CVB}/Lib/C")
endif()

include_directories(${EXTRA_INCLUDE_DIRS})
link_directories(${EXTRA_LIBRARY_DIRS})

set (SOURCES
	include/Module.h
	include/GenICamera.h
	include/ImageViewer.h
	src/GenICamGrab.cpp)
	
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${EXTRA_LIBRARIES})
