
@set build_dir=%1

@if "%build_dir%"=="" goto error

cd ..\dependencies

rem --- box2d ---
rd /s /q box2d\%build_dir%

rem --- zlib ---
rd /s /q zlib\%build_dir%

rem --- libjpeg ---
rd /s /q libjpeg\%build_dir%

rem --- libpng ---
rd /s /q libpng\%build_dir%

rem --- libogg ---
rd /s /q ogg\%build_dir%

rem --- libexpat ---
rd /s /q expat\%build_dir%

rem --- libois ---
rd /s /q ois\%build_dir%

rem --- libopenal ---
rd /s /q openal\%build_dir%

rem --- pybind ---
rd /s /q pybind\%build_dir%

rem --- libtheora ---
rd /s /q theora\%build_dir%

rem --- vorbis ---
rd /s /q vorbis\%build_dir%

rem --- python ---
rd /s /q python\%build_dir%

rem --- pthreads ---
rd /s /q pthreads\%build_dir%

@goto end

:error
@echo Invalid Use

:end
