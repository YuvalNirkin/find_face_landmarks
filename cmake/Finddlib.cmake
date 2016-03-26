# - Find UEye SDK
# Find the native UEye headers and libraries.
#
#  DLIB_INCLUDE_DIRS - where to find uEye.h, etc.
#  DLIB_LIBRARIES    - List of libraries when using UEye.
#  DLIB_FOUND        - True if UEye found.


# Include dir
find_path(dlib_INCLUDE_DIRS dlib
	PATHS 
	$ENV{DLIB_ROOT}
	)

# Finally the library itself
find_library(DLIB_LIB NAMES dlib HINTS $ENV{DLIB_ROOT}/lib)
find_library(DLIB_LIB_DEBUG NAMES dlib_d HINTS $ENV{DLIB_ROOT}/lib)
set(dlib_LIBRARIES optimized ${DLIB_LIB} debug ${DLIB_LIB_DEBUG})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(dlib FOUND_VAR dlib_FOUND
                                  REQUIRED_VARS dlib_INCLUDE_DIRS dlib_LIBRARIES
								  FAIL_MESSAGE "Unable to find dlib! Please set DLIB_ROOT environment variable to the dlib root directory.")


mark_as_advanced(dlib_INCLUDE_DIRS dlib_LIBRARIES )