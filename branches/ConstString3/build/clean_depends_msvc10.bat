@echo off

@echo Starting clean nmake...

rmdir /s /q %CD%\..\dependencies\build_msvc10\Debug
rmdir /s /q %CD%\..\CMakeTemp\Depends_WIN32Debug

@echo Done
@pause

@echo on