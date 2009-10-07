@echo off

@echo Starting build...

@call build_menge build_mingw generator_mingw C:\MinGW\bin\mingw32-make.exe Release

@echo Done
@if NOT "%1"=="/NOPAUSE" pause
