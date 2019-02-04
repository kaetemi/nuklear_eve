
FILE(GLOB SRCS *.c ../src/*.c)
FILE(GLOB HDRS *.h ../src/*.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr/Simulation")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr/MSVC")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr/FT900")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/esd_core")

ADD_DEFINITIONS(-DEVE_GRAPHICS_VM816C)
ADD_DEFINITIONS(-DEVE_PLATFORM_BT8XXEMU)

ADD_EXECUTABLE(nuklear_eve_demo
  ${SRCS}
  ${HDRS}
)

TARGET_LINK_LIBRARIES(nuklear_eve_demo
  ${BT8XXEMU_LIBRARY}
  esd_core
)

IF (WIN32)
  ADD_CUSTOM_COMMAND(TARGET nuklear_eve_demo POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${BT8XXEMU_BINDIR}/bt8xxemu.dll
    ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/bt8xxemu.dll)
  
  ADD_CUSTOM_COMMAND(TARGET nuklear_eve_demo POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${BT8XXEMU_BINDIR}/mx25lemu.dll
    ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/mx25lemu.dll)
ENDIF()