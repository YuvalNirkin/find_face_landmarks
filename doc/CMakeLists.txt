# additional config
set(doxyfile "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
set(DOXYGEN_INPUT_PATH "\"${CMAKE_SOURCE_DIR}/sequence_face_landmarks/sfl\"")
set(DOXYGEN_OUTPUT_PATH "\"${CMAKE_CURRENT_BINARY_DIR}/doxygen\"")

# Write doxygen configuration file
configure_file(Doxyfile.in ${doxyfile} @ONLY)

add_custom_target(doxygen
	COMMAND ${CMAKE_COMMAND} -E echo "Building API Documentation..."
	COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
	COMMAND ${CMAKE_COMMAND} -E echo "Done."
	DEPENDS ${doxyfile}
)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html
	DESTINATION doc
	COMPONENT "docs" OPTIONAL
)