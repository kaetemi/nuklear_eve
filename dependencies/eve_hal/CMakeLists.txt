
FILE(GLOB SRCS *.c Src/*.c Src/Simulation/*.c Src/MSVC/*.c Src/FT900/*.c)
FILE(GLOB HDRS *.h Hdr/*.h Hdr/Simulation/*.h Hdr/MSVC/*.h Hdr/FT900/*.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr/Simulation")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr/MSVC")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr/FT900")

ADD_LIBRARY(eve_hal
  ${SRCS}
  ${HDRS}
)

TARGET_LINK_LIBRARIES(eve_hal
  ${BT8XXEMU_LIBRARY}
)
