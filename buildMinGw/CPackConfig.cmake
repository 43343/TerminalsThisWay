# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BINARY_7Z "OFF")
set(CPACK_BINARY_IFW "OFF")
set(CPACK_BINARY_INNOSETUP "OFF")
set(CPACK_BINARY_NSIS "ON")
set(CPACK_BINARY_NUGET "OFF")
set(CPACK_BINARY_WIX "OFF")
set(CPACK_BINARY_ZIP "OFF")
set(CPACK_BUILD_SOURCE_DIRS "/home/DimonX/TerminalsThisWay;/home/DimonX/TerminalsThisWay/buildMinGw")
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_CREATE_DESKTOP_LINKS "TerminalsThisWay")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "/usr/share/cmake/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "TerminalsThisWay built using CMake")
set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE "ON")
set(CPACK_GENERATOR "NSIS")
set(CPACK_INNOSETUP_ARCHITECTURE "x64")
set(CPACK_INSTALL_CMAKE_PROJECTS "/home/DimonX/TerminalsThisWay/buildMinGw;TerminalsThisWay;ALL;/")
set(CPACK_INSTALL_PREFIX "/usr/local")
set(CPACK_MODULE_PATH "/home/DimonX/TerminalsThisWay/cmake/Modules")
set(CPACK_NSIS_DISPLAY_NAME "TerminalsThisWay")
set(CPACK_NSIS_ENABLE_FINISH_PAGE "ON")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  IfSilent +2
  GoTo NotSilent
  Exec '$INSTDIR\\TerminalsThisWay.exe --update'
  NotSilent:
")
set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS "
  ExecWait 'taskkill /IM TerminalsThisWay.exe /F'
  ExecWait 'taskkill /IM Command.exe /F'
  Sleep 1000
")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  ExecWait 'taskkill /IM TerminalsThisWay.exe /F'
  ExecWait 'taskkill /IM Command.exe /F'
  Sleep 1000
 ")
set(CPACK_NSIS_HELP_LINK "https://github.com/43343/TerminalsThisWay")
set(CPACK_NSIS_INSTALLED_ICON_NAME "TerminalsThisWay.exe")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_MODIFY_PATH "ON")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "TerminalsThisWay.exe")
set(CPACK_NSIS_MUI_ICON "/home/DimonX/TerminalsThisWay/cmake/Templates/../../res/kisspng-square-angle-text-brand-terminal-5ab09bfaa5b5e5.9398888915215237066788.ico")
set(CPACK_NSIS_PACKAGE_NAME "TerminalsThisWay")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/43343/TerminalsThisWay")
set(CPACK_OBJCOPY_EXECUTABLE "/usr/bin/x86_64-w64-mingw32-objcopy")
set(CPACK_OBJDUMP_EXECUTABLE "/usr/bin/x86_64-w64-mingw32-objdump")
set(CPACK_OUTPUT_CONFIG_FILE "/home/DimonX/TerminalsThisWay/buildMinGw/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "/usr/share/cmake/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TerminalsThisWay built using CMake")
set(CPACK_PACKAGE_EXECUTABLES "TerminalsThisWay;TerminalsThisWay")
set(CPACK_PACKAGE_FILE_NAME "TerminalsThisWay-1.0.0-win64")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/43343/TerminalsThisWay")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "TerminalsThisWay")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "TerminalsThisWay")
set(CPACK_PACKAGE_NAME "TerminalsThisWay")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "43343")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_READELF_EXECUTABLE "/usr/bin/x86_64-w64-mingw32-readelf")
set(CPACK_RESOURCE_FILE_LICENSE "/home/DimonX/TerminalsThisWay/cmake/Templates/../../LICENSE")
set(CPACK_RESOURCE_FILE_README "/usr/share/cmake/Templates/CPack.GenericDescription.txt")
set(CPACK_RESOURCE_FILE_WELCOME "/usr/share/cmake/Templates/CPack.GenericWelcome.txt")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_7Z "ON")
set(CPACK_SOURCE_GENERATOR "7Z;ZIP")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/home/DimonX/TerminalsThisWay/buildMinGw/CPackSourceConfig.cmake")
set(CPACK_SOURCE_ZIP "ON")
set(CPACK_STRIP_FILES "YES")
set(CPACK_SYSTEM_NAME "win64")
set(CPACK_THREADS "1")
set(CPACK_TOPLEVEL_TAG "win64")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "/home/DimonX/TerminalsThisWay/buildMinGw/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
