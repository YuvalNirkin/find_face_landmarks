# Validation
if(NOT Boost_FOUND)
	message(STATUS "sfl_cache won't be built because Boost is missing.")
	return()
endif()
if(NOT vsal_FOUND)
	message(STATUS "sfl_cache won't be built because vsal is missing.")
	return()
endif()
if(NOT PROTOBUF_FOUND)
	message(STATUS "sfl_cache won't be built because protobuf is missing.")
	return()
endif()

# Target
if(WIN32)
	link_directories(${Boost_LIBRARY_DIRS})
else()
	link_libraries(${Boost_LIBRARIES})
endif()

add_executable(sfl_cache sfl_cache.cpp)
target_include_directories(sfl_cache PRIVATE 
	${CMAKE_SOURCE_DIR}/sequence_face_landmarks
	${VSAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(sfl_cache PRIVATE 
	sequence_face_landmarks ${VSAL_LIBRARIES})
set_target_properties(sfl_cache PROPERTIES DEBUG_POSTFIX _d)

# Installations
install(TARGETS sfl_cache EXPORT find_face_landmarks-targets DESTINATION bin COMPONENT bin)
set(FFL_TARGETS ${FFL_TARGETS} sfl_cache)
