
FILE(GLOB SRCS *.c)
FILE(GLOB HDRS *.h)
FILE(GLOB COMP Compatibility/*.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})
SOURCE_GROUP("Compatibility" FILES ${COMP})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/esd_core")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/esd_core/Compatibility")

ADD_LIBRARY(esd_core
  ${SRCS}
  ${HDRS}
  ${COMP}
)

TARGET_LINK_LIBRARIES(esd_core
  eve_hal
)
