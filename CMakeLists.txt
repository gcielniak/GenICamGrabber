cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(GenICamApp)

set(CMAKE_CXX_STANDARD 11)

set (EXTRA_INCLUDE_DIRS include)
set (EXTRA_LIBRARY_DIRS "")
set (EXTRA_LIBRARIES "")

# OpenCV
find_package(OpenCV REQUIRED)
set (EXTRA_INCLUDE_DIRS ${EXTRA_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
set (EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ${OpenCV_LIBS})

# PCL
if (true)
find_package(PCL 1.3)
set (EXTRA_INCLUDE_DIRS ${EXTRA_INCLUDE_DIRS} ${PCL_INCLUDE_DIRS})
set (EXTRA_LIBRARY_DIRS ${EXTRA_LIBRARY_DIRS} ${PCL_LIBRARY_DIRS})
set (EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ${PCL_LIBRARIES})
add_definitions(${PCL_DEFINITIONS})
add_definitions(-DHAVE_PCL)
endif()

# include BOOST library if PCL is not being used
if (NOT PCL_FOUND)
find_package(Boost REQUIRED)

set (EXTRA_INCLUDE_DIRS ${EXTRA_INCLUDE_DIRS} ${Boost_INCLUDE_DIR})
set (EXTRA_LIBRARY_DIRS ${EXTRA_LIBRARY_DIRS} $ENV{BOOST_LIBRARYDIR})
set (EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ${Boost_LIBRARIES})
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

# Stemmer Library: GenICam driver
if(EXISTS "$ENV{CVB}/Lib/C")
    set(EXTRA_INCLUDE_DIRS ${EXTRA_INCLUDE_DIRS} "$ENV{CVB}/Lib/C")
    set(EXTRA_LIBRARY_DIRS ${EXTRA_LIBRARY_DIRS} "$ENV{CVB}/Lib/C")
	add_definitions(-DHAVE_GENICAM)
endif()

include_directories(${EXTRA_INCLUDE_DIRS})
link_directories(${EXTRA_LIBRARY_DIRS})

set (SOURCES
	include/Image.h
	include/Camera.h
	include/GenICam/Parameter.h
	include/GenICam/Camera.h
	include/GenICam/AD_130GE.h
	include/GenICam/GenieNano.h
	include/SignalConnector.h
	include/InputDevice.h
	include/CameraManager.h
	include/RGBNGrabber.h
	include/ImageViewer.h
	include/ImageWriter.h
	include/AutoFocus.h
	include/HistogramViewer.h
	include/RGBN2NDVI.h
	src/GenICamApp.cpp)
	
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${EXTRA_LIBRARIES})
