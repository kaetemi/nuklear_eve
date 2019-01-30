# - Try to find Nuklear
# Once done this will define
#  Nuklear_FOUND - System has Nuklear
#  Nuklear_INCLUDE_DIRS - The Nuklear include directories
#  Nuklear_LIBRARIES - The libraries needed to use Nuklear
#  Nuklear_DEFINITIONS - Compiler switches required for using Nuklear

if (!WIN32)
	find_package(PkgConfig)
	pkg_check_modules(PC_Nuklear QUIET nuklear)
endif (!WIN32)
set(Nuklear_DEFINITIONS ${PC_Nuklear_CFLAGS_OTHER})

find_path(
  Nuklear_INCLUDE_DIR nuklear.h
  HINTS ${PC_Nuklear_INCLUDEDIR} ${PC_Nuklear_INCLUDE_DIRS}
  PATH_SUFFIXES nuklear)

set(Nuklear_INCLUDE_DIRS ${Nuklear_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set Nuklear_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Nuklear DEFAULT_MSG Nuklear_INCLUDE_DIR)

mark_as_advanced(Nuklear_INCLUDE_DIR)
