@echo off

@echo Starting clean nmake...

rmdir /s /q %CD%\..\dependencies\build_msvc10

@echo Done
@pause

@echo on