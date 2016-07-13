# Add subdirectories
add_subdirectory(find_face_landmarks_mex)

# Installation
file(GLOB MATLAB_SCRIPTS "*.m")
install(FILES ${MATLAB_SCRIPTS}
		DESTINATION interfaces/matlab)