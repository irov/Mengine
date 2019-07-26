[![Beerpay](https://beerpay.io/irov/Mengine/badge.svg?style=flat)](https://beerpay.io/irov/Mengine)

# Mengine

Mengine is an multiple platforms engine for creating and running graphical games

[![AppVeyor](https://ci.appveyor.com/api/projects/status/w9l085j2o2qn3ua5?svg=true)](https://ci.appveyor.com/project/irov/mengine) [![Travis-CI](https://travis-ci.org/irov/Mengine.svg?branch=master)](https://travis-ci.org/irov/Mengine)


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

Code Guards
-------

[![CodeFactor](https://www.codefactor.io/repository/github/irov/mengine/badge)](https://www.codefactor.io/repository/github/irov/mengine)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/830296ccc09541d4a03d8c3753f25dc4)](https://www.codacy.com/app/irov13/Mengine?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=irov/Mengine&amp;utm_campaign=Badge_Grade)

License
-------
see [LICENSE](https://github.com/irov/Mengine/LICENSE) for more information.