@echo off

@echo Starting dependencies build...

@call build_depends build_mingw generator_mingw C:\MinGW\bin\mingw32-make.exe Release

@echo Done
@pause