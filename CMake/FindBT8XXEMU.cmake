# - Try to find BT8XXEMU
# Once done this will define
#  BT8XXEMU_FOUND - System has BT8XXEMU
#  BT8XXEMU_INCLUDE_DIRS - The BT8XXEMU include directories
#  BT8XXEMU_LIBRARIES - The libraries needed to use BT8XXEMU
#  BT8XXEMU_DEFINITIONS - Compiler switches required for using BT8XXEMU

if (!WIN32)
	find_package(PkgConfig)
	pkg_check_modules(PC_BT8XXEMU QUIET bt8xxemu)
endif (!WIN32)
set(BT8XXEMU_DEFINITIONS ${PC_BT8XXEMU_CFLAGS_OTHER})

find_path(
  BT8XXEMU_INCLUDE_DIR bt8xxemu.h
  HINTS ${PC_BT8XXEMU_INCLUDEDIR} ${PC_BT8XXEMU_INCLUDE_DIRS}
  PATH_SUFFIXES bt8xxemu)

find_library(
  BT8XXEMU_LIBRARY NAMES bt8xxemu libbt8xxemu
  HINTS ${PC_BT8XXEMU_LIBDIR} ${PC_BT8XXEMU_LIBRARY_DIRS}
  PATH_SUFFIXES bt8xxemu)

set(BT8XXEMU_LIBRARIES ${BT8XXEMU_LIBRARY})
set(BT8XXEMU_INCLUDE_DIRS ${BT8XXEMU_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set BT8XXEMU_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(BT8XXEMU DEFAULT_MSG BT8XXEMU_LIBRARY BT8XXEMU_INCLUDE_DIR)

mark_as_advanced(BT8XXEMU_INCLUDE_DIR BT8XXEMU_LIBRARY)
