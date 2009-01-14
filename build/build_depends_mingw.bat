@echo off

@echo Starting dependencies build...

@call build_depends build_mingw generator_mingw mingw.exe Release

@echo Done
@pause