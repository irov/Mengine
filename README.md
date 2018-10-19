# Mengine

Mengine is an multiple platforms engine for creating and running graphical games

|         | Linux  | Mac OS | Windows |
| ------- | ------ | ------ | ------- |
| master  | | | [![AppVeyor](https://ci.appveyor.com/api/projects/status/w9l085j2o2qn3ua5?svg=true)](https://ci.appveyor.com/project/irov/mengine) |

## Windows
Для начала работы нужно скачать исходники библиотек зависимостей
    `Mengine\build\downloads\downloads.bat`
    
Дальше мы можем собираем например дебаг для Visual Studio 15 2017
    `Mengine\build\msvc15\build_depends_msvc15_debug.bat`

и сформировать солюшен
    `Mengine\build\msvc15\make_solution_tools_msvc15_debug.bat`
    
который мы сможем найти в папке
    `Mengine\solutions\solution_msvc15_debug\`
    
Где мы найдем основной проект `Win32Application`
Удачи в сборке, и использованию!


