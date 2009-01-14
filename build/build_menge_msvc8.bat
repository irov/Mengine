@echo off

@echo Starting build...

@call build_menge build_msvc8 generator_nmake nmake.exe Release

@echo Done
@pause