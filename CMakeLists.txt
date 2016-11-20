cmake_minimum_required(VERSION 3.4)
project(find_face_landmarks)

# Set version
set(FFL_MAJOR_VERSION 1)
set(FFL_MINOR_VERSION 1)
set(FIND_FACE_LANDMARKS_VERSION ${FFL_MAJOR_VERSION}.${FFL_MINOR_VERSION})

# Global configurations
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC")
else()
	 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Optional 3rd party components
# ===================================================
option(WITH_VSAL "Video Stream Abstraction Layer" ON)
option(WITH_PROTOBUF "Protocol Buffers - Google's data interchange format" ON)
option(WITH_OPENCV_CONTRIB "OpenCV's extra modules" ON)
option(WITH_QT "Qt" ON)

# Build components
# ===================================================
option(BUILD_SFL_CACHE "Build sfl_cache application" ON)
option(BUILD_SFL_VIEWER "Build sfl_viewer application" ON)
option(BUILD_SFL_TRACK "Build sfl_track application" ON)
option(BUILD_DOCS "Build documentation using Doxygen" ON)
option(BUILD_INTERFACE_MATLAB "Build interface for Matlab" ON)

# Find dependencies
# ===================================================

# dlib
find_package(dlib REQUIRED)

# OpenCV
find_package(OpenCV REQUIRED highgui imgproc imgcodecs features2d)
if(WITH_OPENCV_CONTRIB)
	find_package(OpenCV COMPONENTS face)
endif()

# Boost
if(WIN32)
	set(Boost_USE_STATIC_LIBS ON)	# only find static libs
endif(WIN32)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED filesystem program_options)

# vsal
if(WITH_VSAL)
	find_package(vsal)
endif()

# protobuf
if(WITH_PROTOBUF)
	find_package(protobuf)
endif()

# Qt
if(WITH_QT)
	find_package(Qt5Widgets)
endif()

# Docs
if(BUILD_DOCS)
	find_package(Doxygen)
	if(NOT DOXYGEN_FOUND)
		message(STATUS "The documentation won't be build.")
		set(BUILD_DOCS OFF CACHE BOOL "Build documentation using Doxygen" FORCE)
	endif()
endif()

# Interfaces
if(NOT WITH_VSAL)
	message(STATUS "Matlab interface won't be build because it depends on vsal.")
	set(BUILD_INTERFACE_MATLAB OFF CACHE BOOL "Build interface for Matlab" FORCE)
endif()
if(BUILD_INTERFACE_MATLAB)
	find_package(Matlab COMPONENTS MX_LIBRARY)
	if(NOT Matlab_FOUND)
		message(STATUS "Matlab interface won't be build.")
		set(BUILD_INTERFACE_MATLAB OFF CACHE BOOL "Build interface for Matlab" FORCE)	
	endif()
endif()

# Add subdirectories
# ===================================================
add_subdirectory(sequence_face_landmarks)
set(FFL_TARGETS sequence_face_landmarks)

# cache_face_landmarks
if(BUILD_SFL_CACHE)
	add_subdirectory(sfl_cache)
endif()

# face_landmarks_viewer
if(BUILD_SFL_VIEWER)
	add_subdirectory(sfl_viewer)
endif()

# sfl_track
if(BUILD_SFL_TRACK)
	add_subdirectory(sfl_track)
endif()

if(BUILD_DOCS)
	add_subdirectory(doc)
endif()

# interfaces
if(BUILD_INTERFACE_MATLAB)
	add_subdirectory(interfaces/matlab)
endif()

# Export configuration
# ===================================================

# Add all targets to the build-tree export set
export(TARGETS ${FFL_TARGETS}
  FILE "${PROJECT_BINARY_DIR}/find_face_landmarks-targets.cmake")
  
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE find_face_landmarks)
	 
# Create config files
configure_file(cmake/find_face_landmarks-config.cmake.in
  "${PROJECT_BINARY_DIR}/find_face_landmarks-config.cmake" @ONLY)
configure_file(cmake/find_face_landmarks-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/find_face_landmarks-config-version.cmake" @ONLY)
  
# Install config files
install(FILES
  "${PROJECT_BINARY_DIR}/find_face_landmarks-config.cmake"
  "${PROJECT_BINARY_DIR}/find_face_landmarks-config-version.cmake"
  DESTINATION "cmake" COMPONENT dev)
  
# Install the export set for use with the install-tree
install(EXPORT find_face_landmarks-targets DESTINATION cmake COMPONENT dev)
  
  
#install(FILES ${CMAKE_SOURCE_DIR}/find_face_landmarks.txt DESTINATION mex COMPONENT mex RENAME find_face_landmarks.m)
#install(FILES ${CMAKE_SOURCE_DIR}/show_face_landmarks.m DESTINATION utilities COMPONENT utilities)
#install(FILES ${CMAKE_SOURCE_DIR}/find_face_landmarks_setup.m DESTINATION .)
