@echo off

@echo Starting clean nmake...

rmdir /s /q %CD%\..\dependencies\build_mingw

@echo Done
@pause

@echo on