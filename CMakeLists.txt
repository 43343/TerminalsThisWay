cmake_minimum_required(VERSION 3.16)

project(TerminalsThisWay VERSION 0.0.4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "res/app_icon.rc")
endif()

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

configure_file(
     "src/version.h.in"
     "../src/version.h"
   )

   set(SourcesWin
    src/main.cpp
    src/application.h
    src/win/application.cpp
    src/win/Window/trayWindow.cpp
    src/win/Window/trayWindow.h
    src/win/Window/settings.cpp
    src/win/Window/settings.h
    src/win/Window/GUI/inputField.cpp
    src/win/Window/GUI/inputField.h
    src/win/Window/GUI/toggle.cpp
    src/win/Window/GUI/toggle.h
    src/win/Window/GUI/InputKeyBind.cpp
    src/win/Window/GUI/InputKeyBind.h
    src/win/Window/GUI/text.cpp
    src/win/Window/GUI/text.h
    src/win/Window/GUI/button.cpp
    src/win/Window/GUI/button.h
    src/win/Window/GUI/image.cpp
    src/win/Window/GUI/image.h
    src/win/Window/parameterInputWindow.cpp
    src/win/Window/parameterInputWindow.h
    src/win/Window/about.cpp
    src/win/Window/about.h
    src/win/Window/modalWindow.cpp
    src/win/Window/modalWindow.h
    src/win/updater.cpp
    src/win/updater.h
    src/win/Config/config.h
    src/win/Config/configManager.cpp
    src/win/Config/configManager.h
    src/win/Utility/utility.cpp
    src/win/Utility/utility.h
    src/win/Utility/json.hpp
    src/win/GetText.cpp
    src/win/GetText.h
    src/win/Window/chooseFolder.cpp
    src/win/Window/chooseFolder.h
    src/win/terminal.cpp
    src/win/terminal.h
    src/version.h
)
set(SourcesLinux
src/main.cpp
src/application.h
src/version.h
src/linux/application.cpp
)

if(LINUX)
  find_package(X11)
  add_executable(TerminalsThisWay ${SourcesLinux})
  target_link_libraries(TerminalsThisWay ${X11_LIBRARIES})
  target_include_directories(TerminalsThisWay PRIVATE ${X11_INCLUDE_DIR})
endif()
if(WIN32)
    list(APPEND SourcesWin ${APP_ICON_RESOURCE_WINDOWS})
    add_executable(TerminalsThisWay ${SourcesWin})
    target_link_libraries(TerminalsThisWay PRIVATE wininet)
endif()

install(TARGETS TerminalsThisWay DESTINATION .)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})

include(cmake/Templates/CPackConfig.cmake)