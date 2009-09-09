
@set build_dir=%1

@if "%build_dir%"=="" goto error

rd /s /q ..\%build_dir%

@goto end

:error
@echo Invalid Use

:end
