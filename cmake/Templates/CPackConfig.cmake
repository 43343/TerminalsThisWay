set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
 set(CPACK_PACKAGE_EXECUTABLES "TerminalsThisWay" "TerminalsThisWay")
 set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/43343/TerminalsThisWay")
 set(CPACK_NSIS_HELP_LINK "https://github.com/43343/TerminalsThisWay")
 set(CPACK_PACKAGE_VENDOR "43343")
 set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/43343/TerminalsThisWay")

 set (CPACK_CREATE_DESKTOP_LINKS "TerminalsThisWay.exe")
 set (CPACK_PACKAGE_INSTALL_DIRECTORY "TerminalsThisWay")
 set (CPACK_NSIS_MUI_ICON
  "${CMAKE_CURRENT_LIST_DIR}/../../res/kisspng-square-angle-text-brand-terminal-5ab09bfaa5b5e5.9398888915215237066788.ico")
  set (CPACK_RESOURCE_FILE_LICENSE
  "${CMAKE_CURRENT_LIST_DIR}/../../LICENSE")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "TerminalsThisWay.exe")
  set (CPACK_CREATE_DESKTOP_LINKS "TerminalsThisWay")
  set(CPACK_STRIP_FILES YES)
 set(CPACK_NSIS_ENABLE_FINISH_PAGE ON)
 set(CPACK_NSIS_MUI_FINISHPAGE_RUN "TerminalsThisWay.exe")
set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS "
  ExecWait 'taskkill /IM TerminalsThisWay.exe /F'
  Sleep 1000
")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  ExecWait 'taskkill /IM TerminalsThisWay.exe /F'
  Sleep 1000
 ")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  IfSilent +2
  GoTo NotSilent
  ExecWait '$INSTDIR\\TerminalsThisWay.exe --update'
  NotSilent:
")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)

include(CPack)