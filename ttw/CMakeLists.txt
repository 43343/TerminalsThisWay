cmake_minimum_required(VERSION 3.16)

project(TerminalsThisWay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "../res/app_icon.rc")
endif()
set(SourcesWin
    main.cpp
    application.h
    win/application.cpp
    win/Window/trayWindow.cpp
    win/Window/trayWindow.h
    win/Window/settings.cpp
    win/Window/settings.h
    win/Window/GUI/inputField.cpp
    win/Window/GUI/inputField.h
    win/Window/GUI/toggle.cpp
    win/Window/GUI/toggle.h
    win/Window/GUI/InputKeyBind.cpp
    win/Window/GUI/InputKeyBind.h
    win/Window/GUI/text.cpp
    win/Window/GUI/text.h
    win/Window/GUI/button.cpp
    win/Window/GUI/button.h
    win/Window/GUI/image.cpp
    win/Window/GUI/image.h
    win/Window/parameterInputWindow.cpp
    win/Window/parameterInputWindow.h
    win/Window/about.cpp
    win/Window/about.h
    win/Window/modalWindow.cpp
    win/Window/modalWindow.h
    win/updater.cpp
    win/updater.h
    win/Config/config.h
    win/Config/configManager.cpp
    win/Config/configManager.h
    win/Utility/utility.cpp
    win/Utility/utility.h
    win/Utility/json.hpp
    win/getText.cpp
    win/getText.h
    win/getHighlightedText.cpp
    win/getHighlightedText.h
    win/Window/chooseFolder.cpp
    win/Window/chooseFolder.h
    win/terminal.cpp
    win/terminal.h
    win/simulating.cpp
    win/simulating.h
    version.h
)
if(WIN32)
    list(APPEND SourcesWin ${APP_ICON_RESOURCE_WINDOWS})
    add_executable(TerminalsThisWay ${SourcesWin})
    target_link_libraries(TerminalsThisWay PRIVATE winhttp)


endif()
