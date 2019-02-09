
FILE(GLOB SRCS *.c)
FILE(GLOB HDRS *.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/esd_core")

ADD_LIBRARY(esd_core
  ${SRCS}
  ${HDRS}
)

TARGET_LINK_LIBRARIES(esd_core
  eve_hal
)
