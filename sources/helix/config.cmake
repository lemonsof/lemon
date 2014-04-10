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

CHECK_INCLUDE_FILES (sys/epoll.h HELIX_HAS_EPOLL_H)