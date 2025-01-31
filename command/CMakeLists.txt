cmake_minimum_required(VERSION 3.16)

project(Command LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
set(SourcesWin
    main.cpp
    application.h
    application.cpp
)
if(WIN32)
    add_executable(Command ${SourcesWin})

endif()
