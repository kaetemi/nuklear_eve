
FILE(GLOB SRCS *.c ../src/*.c)
FILE(GLOB HDRS *.h ../src/*.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})

FILE(GLOB EVE_HAL ../dependencies/eve_hal/Hdr/*.h ../dependencies/eve_hal/Hdr/Simulation/*.h ../dependencies/eve_hal/Src/*.c ../dependencies/eve_hal/Src/Simulation/*.c)
SOURCE_GROUP("EVE HAL" FILES ${EVE_HAL})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr/MSVC")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Hdr/Simulation")

ADD_DEFINITIONS(-DEVE_GRAPHICS_VM816C)
ADD_DEFINITIONS(-DEVE_PLATFORM_BT8XXEMU)

ADD_EXECUTABLE(nuklear_eve_demo
  ${SRCS}
  ${HDRS}
  ${EVE_HAL}
)

TARGET_LINK_LIBRARIES(nuklear_eve_demo
  ${BT8XXEMU_LIBRARY}
)
