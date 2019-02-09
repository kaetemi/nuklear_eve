
FILE(GLOB SRCS *.c)
FILE(GLOB HDRS *.h Hdr/*.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr")

ADD_LIBRARY(eve_hal
  ${SRCS}
  ${HDRS}
)
