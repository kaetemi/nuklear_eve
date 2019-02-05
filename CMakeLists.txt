########################################################################
# CMake project
PROJECT("nuklear_eve")

# Minimum required 2.8
CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)
# set (CMAKE_VERBOSE_MAKEFILE ON)

IF (POLICY CMP0020)
	CMAKE_POLICY(SET CMP0020 NEW)
ENDIF ()

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

########################################################################
# Ensure that we are not building in our source directories.

SET(Build_Dir_OK "TRUE")
STRING(REGEX MATCH "^${CMAKE_SOURCE_DIR}" In_Sub_Dir ${CMAKE_BINARY_DIR})
IF (In_Sub_Dir)
  STRING(REGEX MATCH "^${CMAKE_SOURCE_DIR}/[Bb]uild" In_Build_Dir ${CMAKE_BINARY_DIR})
  IF (NOT In_Build_Dir)
    SET(Build_Dir_OK "FALSE")
  ENDIF ()
ENDIF ()

IF (NOT Build_Dir_OK)
  MESSAGE(FATAL_ERROR "You must run cmake from a directory that is not in your source tree, or that is in a special subdirectory of the tree whose name begins with 'build'.")
ENDIF ()

########################################################################
# Set up the basic build environment

IF (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This messes up
  # differentiation between debug and release builds.
  SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF ()


SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/") 

########################################################################
# Strip binary in release mode

IF (CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")
ENDIF ()

########################################################################

SET(PC_Nuklear_INCLUDEDIR "${CMAKE_SOURCE_DIR}/dependencies/nuklear")

FIND_PACKAGE(Nuklear REQUIRED)

INCLUDE_DIRECTORIES(${Nuklear_INCLUDE_DIRS})

########################################################################

SET(PC_BT8XXEMU_INCLUDEDIR "${CMAKE_SOURCE_DIR}/dependencies/bt8xxemu/include")
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(PC_BT8XXEMU_LIBDIR "${CMAKE_SOURCE_DIR}/dependencies/bt8xxemu/lib64")
ELSE()
  SET(PC_BT8XXEMU_LIBDIR "${CMAKE_SOURCE_DIR}/dependencies/bt8xxemu/lib")
ENDIF()

FIND_PACKAGE(BT8XXEMU REQUIRED)

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(BT8XXEMU_BINDIR "${CMAKE_SOURCE_DIR}/dependencies/bt8xxemu/bin64")
ELSE()
  SET(BT8XXEMU_BINDIR "${CMAKE_SOURCE_DIR}/dependencies/bt8xxemu/bin")
ENDIF()

LINK_DIRECTORIES(${BT8XXEMU_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${BT8XXEMU_INCLUDE_DIRS})

########################################################################

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

########################################################################

SET(NUKLEAR_EVE_DEMO_GRAPHICS "VM816C" CACHE STRING "Demo Graphics Module")
SET_PROPERTY(CACHE NUKLEAR_EVE_DEMO_GRAPHICS PROPERTY STRINGS "VM816C" "VM810C")
 
SET(NUKLEAR_EVE_DEMO_PLATFORM "BT8XXEMU" CACHE STRING "Demo Platform Target")
SET_PROPERTY(CACHE NUKLEAR_EVE_DEMO_PLATFORM PROPERTY STRINGS "BT8XXEMU" "FT4222")

########################################################################

IF(NUKLEAR_EVE_DEMO_GRAPHICS STREQUAL "VM816C")
  ADD_DEFINITIONS(-DEVE_GRAPHICS_VM816C)
ELSEIF(NUKLEAR_EVE_DEMO_GRAPHICS STREQUAL "VM810C")
  ADD_DEFINITIONS(-DEVE_GRAPHICS_VM810C)
ENDIF()

IF(NUKLEAR_EVE_DEMO_PLATFORM STREQUAL "BT8XXEMU")
  ADD_DEFINITIONS(-DEVE_PLATFORM_BT8XXEMU)
ELSEIF(NUKLEAR_EVE_DEMO_PLATFORM STREQUAL "FT4222")
  ADD_DEFINITIONS(-DEVE_PLATFORM_FT4222)
ENDIF()

########################################################################

ADD_SUBDIRECTORY(dependencies/eve_hal)
ADD_SUBDIRECTORY(dependencies/esd_core)
ADD_SUBDIRECTORY(demo)

########################################################################
