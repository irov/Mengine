@echo off

@echo Starting dependencies build debug configuration...
@call build_depends build_msvc8 generator_nmake nmake.exe Debug

@echo Done
@pause