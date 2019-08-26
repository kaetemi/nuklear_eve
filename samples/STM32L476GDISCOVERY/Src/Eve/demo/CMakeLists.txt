
FILE(GLOB SRCS *.c ../src/*.c)
FILE(GLOB HDRS *.h ../src/*.h ../dependencies/nuklear/*.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/esd_core")

ADD_EXECUTABLE(nuklear_eve_demo
  ${SRCS}
  ${HDRS}
)

TARGET_LINK_LIBRARIES(nuklear_eve_demo
  esd_core
)

IF(NUKLEAR_EVE_DEMO_PLATFORM STREQUAL "BT8XXEMU")
  TARGET_LINK_LIBRARIES(nuklear_eve_demo
    ${BT8XXEMU_LIBRARY}
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
ELSEIF(NUKLEAR_EVE_DEMO_PLATFORM STREQUAL "FT4222")
  TARGET_LINK_LIBRARIES(nuklear_eve_demo
    LibFT4222
    ftd2xx
  )
  IF (WIN32)
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
      ADD_CUSTOM_COMMAND(TARGET nuklear_eve_demo POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/dependencies/ft4222/lib/amd64/LibFT4222-64.dll
        ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/LibFT4222-64.dll)
    ELSE()
      ADD_CUSTOM_COMMAND(TARGET nuklear_eve_demo POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/dependencies/ft4222/lib/i386/LibFT4222.dll
        ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/LibFT4222.dll)
    ENDIF()
  ENDIF()
ELSEIF(NUKLEAR_EVE_DEMO_PLATFORM STREQUAL "MPSSE")
  TARGET_LINK_LIBRARIES(nuklear_eve_demo
    libMPSSE
    ftd2xx
  )
  IF (WIN32)
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
      ADD_CUSTOM_COMMAND(TARGET nuklear_eve_demo POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/dependencies/mpsse_spi/lib/windows/x64/libMPSSE.dll
        ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/libMPSSE.dll)
    ELSE()
      ADD_CUSTOM_COMMAND(TARGET nuklear_eve_demo POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/dependencies/mpsse_spi/lib/windows/i386/libMPSSE.dll
        ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/libMPSSE.dll)
    ENDIF()
  ENDIF()
ENDIF()

