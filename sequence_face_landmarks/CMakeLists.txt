# Source
set(SFL_SRC sequence_face_landmarks.cpp face_tracker.cpp utilities.cpp)
set(SFL_INCLUDE sfl/sequence_face_landmarks.h sfl/face_tracker.h sfl/utilities.h)
if(WITH_PROTOBUF)
	set(PROTO_FILES sequence_face_landmarks.proto)
	protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})
	set(SFL_SRC ${SFL_SRC} ${PROTO_SRCS} ${PROTO_HDRS} ${PROTO_FILES})
	add_definitions(-DWITH_PROTOBUF)
endif()

# Target
add_library(sequence_face_landmarks ${SFL_SRC} ${SFL_INCLUDE})
target_include_directories(sequence_face_landmarks PUBLIC 
	${OpenCV_INCLUDE_DIRS} ${dlib_INCLUDE_DIRS})
target_link_libraries(sequence_face_landmarks PUBLIC 
	${OpenCV_LIBS} ${dlib_LIBRARIES})
if(WITH_PROTOBUF)
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