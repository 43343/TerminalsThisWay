set(CMAKE_SYSTEM_NAME Windows)

set(MINGW_TARGET_ARCH "x86" CACHE STRING "Target architecture (x86/x64)")
set_property(CACHE MINGW_TARGET_ARCH PROPERTY STRINGS "x86" "x64")

if(NOT DEFINED MINGW_TARGET_ARCH)
    message(FATAL_ERROR "Architecture not defined! Use -DMINGW_TARGET_ARCH=x86 или x64")
elseif(NOT MINGW_TARGET_ARCH MATCHES "^(x86|x64)$")
    message(FATAL_ERROR "Invalid architecture: ${MINGW_TARGET_ARCH}. Use x86/x64")
endif()
message(STATUS "Selected architecture: ${MINGW_TARGET_ARCH}")

if(MINGW_TARGET_ARCH STREQUAL "x64")
    set(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)
    set(CMAKE_RC_COMPILER /usr/bin/x86_64-w64-mingw32-windres)
elseif(MINGW_TARGET_ARCH STREQUAL "x86")
    set(CMAKE_C_COMPILER /usr/bin/i686-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/i686-w64-mingw32-g++)
    set(CMAKE_RC_COMPILER /usr/bin/i686-w64-mingw32-windres)
endif()


set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_CROSSCOMPILING TRUE)
