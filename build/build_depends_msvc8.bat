@echo off

@echo Starting dependencies build...

@call build_depends build_msvc8 generator_nmake nmake.exe Release

@echo Done
@pause