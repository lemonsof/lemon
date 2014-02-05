include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckCXXSourceRuns)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)

include(${LEMON_PACKAGE_DIRECTORY}/inttypes.cmake)

if(WIN32)
	include(${LEMON_PACKAGE_DIRECTORY}/win32.cmake)
else()
	include(${LEMON_PACKAGE_DIRECTORY}/unix.cmake)
endif()

CHECK_INCLUDE_FILES (inttypes.h SOF_HAS_INTTYPES_H)
CHECK_INCLUDE_FILES (unistd.h SOF_HAS_UNISTD_H)
CHECK_INCLUDE_FILES (stdint.h SOF_HAS_STDINT_H)
CHECK_INCLUDE_FILES (stdbool.h SOF_HAS_STDBOOL_H)