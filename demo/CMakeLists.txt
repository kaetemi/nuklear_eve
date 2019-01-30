
FILE(GLOB SRCS *.c ../src/*.c)
FILE(GLOB HDRS *.h ../src/*.h)
SOURCE_GROUP("" FILES ${SRCS} ${HDRS})

FILE(GLOB EVE_HAL ../dependencies/eve_hal/Hdr/*.h ../dependencies/eve_hal/Hdr/Msvc_Emulator/*.h ../dependencies/eve_hal/Src/*.c ../dependencies/eve_hal/Src/Msvc_Emulator/*.c)
SOURCE_GROUP("EVE HAL" FILES ${EVE_HAL})

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/dependencies/eve_hal/Msvc_Emulator")

ADD_EXECUTABLE(nuklear_eve_demo
  ${SRCS}
  ${HDRS}
  ${EVE_HAL}
)

TARGET_LINK_LIBRARIES(nuklear_eve_demo
  ${BT8XXEMU_LIBRARY}
)
