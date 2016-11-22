# Validation
if(NOT Boost_FOUND)
	message(STATUS "sfl_viewer won't be built because Boost is missing.")
	return()
endif()
if(NOT vsal_FOUND)
	message(STATUS "sfl_viewer won't be built because vsal is missing.")
	return()
endif()
if(NOT PROTOBUF_FOUND)
	message(STATUS "sfl_viewer won't be built because protobuf is missing.")
	return()
endif()
if(NOT Qt5Widgets_FOUND)
	message(STATUS "sfl_viewer won't be built because Qt is missing.")
	return()
endif()

# Source
set(SFL_VIEWER_SRC sfl_viewer_main.cpp sfl_viewer.cpp sfl_viewer_states.cpp)
set(SFL_VIEWER_HDR sfl_viewer.h sfl_viewer_states.h)
qt5_wrap_cpp(SFL_VIEWER_HDR_MOC ${SFL_VIEWER_HDR})
qt5_wrap_ui(SFL_VIEWER_UI_MOC sfl_viewer.ui)
qt5_add_resources(SFL_VIEWER_QRC sfl_viewer.qrc)

if(MSVC)
	set(SFL_VIEWER_SRC ${SFL_VIEWER_SRC} sfl_viewer.rc)
endif()

# Target
if(WIN32)
	link_directories(${Boost_LIBRARY_DIRS})
else()
	link_libraries(${Boost_LIBRARIES})
endif()

add_executable(sfl_viewer WIN32 ${SFL_VIEWER_SRC} ${SFL_VIEWER_HDR}
	${SFL_VIEWER_HDR_MOC} ${SFL_VIEWER_UI_MOC} ${SFL_VIEWER_QRC})
target_include_directories(sfl_viewer PRIVATE 
	${CMAKE_SOURCE_DIR}/sequence_face_landmarks
	${CMAKE_SOURCE_DIR}/sfl_viewer
	${CMAKE_BINARY_DIR}/sfl_viewer
	${VSAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(sfl_viewer PRIVATE 
	sequence_face_landmarks ${VSAL_LIBRARIES} Qt5::Widgets)
set_target_properties(sfl_viewer PROPERTIES DEBUG_POSTFIX _d)

# Installations
install(TARGETS sfl_viewer EXPORT find_face_landmarks-targets DESTINATION bin COMPONENT bin)
set(FFL_TARGETS ${FFL_TARGETS} sfl_viewer)
