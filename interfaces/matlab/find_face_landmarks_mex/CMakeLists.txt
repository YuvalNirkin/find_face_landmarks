# Validation
if(NOT Boost_FOUND)
	message(STATUS "find_face_landmarks won't be built because Boost is missing.")
	return()
endif()

# Validation
if(NOT vsal_FOUND)
	message(STATUS "find_face_landmarks won't be built because vsal is missing.")
	return()
endif()

# Add mex target
if(WIN32)
	link_directories(${Boost_LIBRARY_DIRS})
else()
	link_libraries(${Boost_LIBRARIES})
endif()

matlab_add_mex(NAME find_face_landmarks
	SRC find_face_landmarks_mex.cpp MxArray.hpp MxArray.cpp
	DOCUMENTATION ${CMAKE_CURRENT_SOURCE_DIR}/find_face_landmarks.txt)
target_include_directories(find_face_landmarks PRIVATE 
	${CMAKE_SOURCE_DIR}/sequence_face_landmarks
	${Matlab_INCLUDE_DIRS} ${VSAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(find_face_landmarks sequence_face_landmarks 
	${Matlab_LIBRARIES} ${VSAL_LIBRARIES})

# Installations
if(MSVC)
  INSTALL(TARGETS find_face_landmarks
    LIBRARY DESTINATION lib COMPONENT matlab
    RUNTIME DESTINATION interfaces/matlab COMPONENT matlab)
else()
  INSTALL(TARGETS find_face_landmarks
    LIBRARY DESTINATION interfaces/matlab COMPONENT matlab
    RUNTIME DESTINATION bin COMPONENT matlab)
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/find_face_landmarks.txt
	DESTINATION interfaces/matlab COMPONENT matlab RENAME find_face_landmarks.m)