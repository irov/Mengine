@echo off

@echo Starting clean nmake...

rmdir /s /q %CD%\..\dependencies\build_msvc8

@echo Done
@pause

@echo on