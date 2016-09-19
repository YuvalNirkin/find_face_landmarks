# Validation
if(NOT PROTOBUF_FOUND)
	message(STATUS "sequence_face_landmarks will be built without loading and saving support because protobuf is missing.")
endif()
if(NOT WITH_OPENCV_CONTRIB OR NOT TARGET opencv_face)
	message(STATUS "sequence_face_landmarks will be built without LBP tracker because OpenCV Contrib is missing.")
endif()

# Source
set(SFL_SRC sequence_face_landmarks.cpp face_tracker_brisk.cpp face_tracker_lbp.cpp utilities.cpp)
set(SFL_INCLUDE sfl/sequence_face_landmarks.h sfl/face_tracker.h sfl/utilities.h)
if(PROTOBUF_FOUND)
	set(PROTO_FILES sequence_face_landmarks.proto)
	protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})
	set(SFL_SRC ${SFL_SRC} ${PROTO_SRCS} ${PROTO_HDRS} ${PROTO_FILES})
	add_definitions(-DWITH_PROTOBUF)
endif()
if(WITH_OPENCV_CONTRIB AND TARGET opencv_face)
	add_definitions(-DWITH_OPENCV_CONTRIB)
endif()

# Target
if(WIN32)
	link_directories(${Boost_LIBRARY_DIRS})
else()
	link_libraries(${Boost_LIBRARIES})
endif()
add_library(sequence_face_landmarks ${SFL_SRC} ${SFL_INCLUDE})
target_include_directories(sequence_face_landmarks PUBLIC 
	${Boost_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${dlib_INCLUDE_DIRS})
target_link_libraries(sequence_face_landmarks PUBLIC 
	${OpenCV_LIBS} ${dlib_LIBRARIES})
if(PROTOBUF_FOUND)
	target_include_directories(sequence_face_landmarks PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
	target_include_directories(sequence_face_landmarks PUBLIC ${PROTOBUF_INCLUDE_DIRS})
	target_link_libraries(sequence_face_landmarks PUBLIC ${PROTOBUF_LIBRARIES})
endif()
set_target_properties(sequence_face_landmarks PROPERTIES DEBUG_POSTFIX _d)

# Installations
install(TARGETS sequence_face_landmarks
	 EXPORT find_face_landmarks-targets
     RUNTIME DESTINATION bin COMPONENT dev
     LIBRARY DESTINATION lib COMPONENT dev
     ARCHIVE DESTINATION lib COMPONENT dev)
install(FILES ${SFL_INCLUDE} DESTINATION include/sfl)
