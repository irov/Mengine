@echo off

cd ..\dependencies

rem --- zlib ---
rd /s /q zlib\mingw_build

rem --- libpng ---
rd /s /q libpng\mingw_build

rem --- libogg ---
rd /s /q ogg\mingw_build

rem --- libexpat ---
rd /s /q expat\mingw_build

rem --- libois ---
rd /s /q ois\mingw_build

rem --- libopenal ---
rd /s /q openal\mingw_build

rem --- pybind ---
rd /s /q pybind\mingw_build

rem --- libtheora ---
rd /s /q theora\mingw_build

rem --- vorbis ---
rd /s /q vorbis\mingw_build

if NOT "%1"=="/nopause" pause

