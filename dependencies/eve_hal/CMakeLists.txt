
FILE(GLOB SRCS *.c)
FILE(GLOB HDRS *.h Hdr/*.h)
FILE(GLOB COMP Hdr/*.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})
SOURCE_GROUP("Compatibility" FILES ${COMP})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr")

ADD_LIBRARY(eve_hal
  ${SRCS}
  ${HDRS}
  ${COMP}
)
