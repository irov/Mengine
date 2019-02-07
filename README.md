# Mengine

Mengine is an multiple platforms engine for creating and running graphical games

|         | Linux  | Mac OS | Windows |
| ------- | ------ | ------ | ------- |
| master  | | [![Travis-CI](https://travis-ci.org/irov/Mengine.svg?branch=master)](https://travis-ci.org/irov/Mengine) | [![AppVeyor](https://ci.appveyor.com/api/projects/status/w9l085j2o2qn3ua5?svg=true)](https://ci.appveyor.com/project/irov/mengine) |


Проект использует [CMake](https://cmake.org/) в качестве cистемы сборки проекта

## Windows
Для начала работы нужно скачать исходники библиотек зависимостей

`Mengine\build\downloads\downloads.bat`
    
Дальше мы можем собрать например дебаг для Visual Studio 15 2017

`Mengine\build\msvc15\build_depends_msvc15_debug.bat`

и сформировать солюшен

`Mengine\build\msvc15\make_solution_msvc15_debug.bat`
    
который мы сможем найти в папке

`Mengine\solutions\solution_msvc15_debug\Mengine_WIN32.sln`
    
Где мы найдем основной проект `Win32Application`

Удачи в сборке, и использованию!

[![CodeFactor](https://www.codefactor.io/repository/github/irov/mengine/badge)](https://www.codefactor.io/repository/github/irov/mengine)
