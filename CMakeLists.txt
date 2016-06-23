cmake_minimum_required(VERSION 3.4)
project(find_face_landmarks)

# Global configurations
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
endif()

# Find dependencies
# ===================================================
find_package(OpenCV REQUIRED highgui imgproc imgcodecs)
find_package(vsal REQUIRED)
find_package(dlib REQUIRED)
find_package(Matlab REQUIRED MX_LIBRARY)

# Add mex target
# ===================================================
include_directories(${OpenCV_INCLUDE_DIRS} ${VSAL_INCLUDE_DIRS} ${dlib_INCLUDE_DIRS} ${Matlab_INCLUDE_DIRS})
matlab_add_mex(NAME find_face_landmarks
	SRC find_face_landmarks_mex.cpp find_face_landmarks.h find_face_landmarks.cpp MxArray.hpp MxArray.cpp
	DOCUMENTATION ${CMAKE_SOURCE_DIR}/find_face_landmarks.txt)
target_link_libraries(find_face_landmarks ${OpenCV_LIBS} ${VSAL_LIBRARIES} ${dlib_LIBRARIES} ${Matlab_LIBRARIES})
#install(TARGETS find_face_landmarks EXPORT find_face_landmarks-targets DESTINATION mex COMPONENT mex)
if(MSVC)
  INSTALL(TARGETS find_face_landmarks
    LIBRARY DESTINATION lib COMPONENT mex
    RUNTIME DESTINATION mex COMPONENT mex)
else()
  INSTALL(TARGETS find_face_landmarks
    LIBRARY DESTINATION mex COMPONENT mex
    RUNTIME DESTINATION bin COMPONENT mex)
endif()
	 
install(FILES ${CMAKE_SOURCE_DIR}/find_face_landmarks.txt DESTINATION mex COMPONENT mex RENAME find_face_landmarks.m)
install(FILES ${CMAKE_SOURCE_DIR}/show_face_landmarks.m DESTINATION utilities COMPONENT utilities)
install(FILES ${CMAKE_SOURCE_DIR}/find_face_landmarks_setup.m DESTINATION .)
