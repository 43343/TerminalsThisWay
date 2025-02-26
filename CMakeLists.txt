cmake_minimum_required(VERSION 3.16)

if(MINGW)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

project(TerminalsThisWay VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "res/app_icon.rc")
endif()

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()
if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()

configure_file(
     "ttw/version.h.in"
     "../ttw/version.h"
   )

add_subdirectory(command)
add_subdirectory(ttw)

#install(TARGETS TerminalsThisWay Command DESTINATION .)

include(cmake/Templates/CPackConfig.cmake)
