# Mengine

Mengine is an multiple platforms engine for creating and running graphical games

[![AppVeyor](https://ci.appveyor.com/api/projects/status/w9l085j2o2qn3ua5?svg=true)](https://ci.appveyor.com/project/irov/mengine) [![Travis-CI](https://travis-ci.org/irov/Mengine.svg?branch=master)](https://travis-ci.org/irov/Mengine) [![CircleCI](https://circleci.com/gh/irov/Mengine.svg?style=svg)](https://app.circleci.com/pipelines/github/irov/Mengine)


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

Для того что бы начал работать [cppcheck](http://cppcheck.sourceforge.net/) нужно в .git/config добавить 

```
[include]
	path = ../.gitconfig
```

## Projects

Игры которые разрабатываются или были выпущены на данном игровом движке

Mobile
-------

[Evil Monkey](https://play.google.com/store/apps/details?id=org.Wonderland.EvilMonkey) There’s a very hungry and very Evil Monkey under the banana tree! Try to steal those sweet, sweet bananas without drawing the monkey’s wrath. Shake bananas from the tree and if you get too greedy – you’re in big trouble!

[The Dungeon Chronicles](https://www.wonderland-games.com/projects/the-dungeon-chronicles) Fight the creatures of darkness with their own weapons to make your way through the gloomy dungeons and save princess from the Skeleton King!

Desktop
-------

[Christmas Adventure: Candy Storm](https://store.steampowered.com/app/456430/Christmas_Adventure_Candy_Storm) Lisa’s grandfather, Santa Claus, has asked for her help delivering Christmas presents to a remote town. Trying to shorten her way, she drives through an old forest when suddenly her car turns into candies!! Moreover, everything around is scattered with candies and covered with other sweets! Help Lisa investigate what’s happened and help fix her car so she can make it to town in time to save Christmas!

[Christmas Adventures: A Winter Night's Dream](https://store.steampowered.com/app/1490790/Christmas_Adventures_A_Winter_Nights_Dream) Shiny snowflakes, jingling bells and magic in the air… All this might not happen, because dark forces want to destroy our Christmas celebrations… again. We need your help!

[The Wisbey Mystery](https://store.steampowered.com/app/575960/The_Wisbey_Mystery) For many years locals have avoided the manor by the woodside. Its dark and dilapidated walls keep a terrible secret about the unsolved murder of the sheriff and his wife. The mystery continues when a young journalist, writing a story about the incident, disappears under murky circumstances. Maybe the maniac’s diary he found earlier has something to do with it… Solve the manor’s mystery and save the journalist before it’s too late!

[Amulet of Dreams](https://store.steampowered.com/app/476920/Amulet_of_Dreams) Alison’s sudden illness was a shock to everyone. Doctors do not understand what was happening to her, and what caused her illness. Family and doctors are desperate, and only faithful friend never lost hope. Faithful friendfriend. An ordinary little boy named Aiden. Aiden – it’s you.

[The Cursed Island: Mask of Baragus](https://www.bigfishgames.com/games/8368/the-cursed-island-mask-of-baragus) Overcoming considerable obstacles Casey arrives on the island to discover, not only has her father disappeared but his entire team has also gone missing! Word quickly spreads that Stromberg, one of the team members, attempted to invoke the spirit Baragus using a mysterious and ancient artifact.

[Into the Haze](https://www.bigfishgames.com/games/7727/into-the-haze) Rescue a young girl from the clutches of an ancient demon in Into the Haze! Guide your character through the mist of a hair-raising supernatural adventure, where she'll encounter an ancient fog demon summoned by her sister-in-law in a magic ritual gone wrong.

[1001 Nights: The Adventures of Sindbad](https://www.bigfishgames.com/games/5394/1001-nights-the-adventures-of-sindbad) Help the lovely Princess Chalida locate the seven gems of her royal crown and break the curse that has trapped her in a body of wood!

[Marie Antoinette and the Disciples of Loki](https://www.bigfishgames.com/games/6978/marie-antoinette-and-the-disciples-of-loki) The year is 1787 and the French monarchy is living its last years of peace. In Vienna, Mozart is just about to finish his latest opera. Far to the north, in Iceland, a tremendous volcano eruption has already changed the face of Europe forever. What is the link between those events?

Code Style
-------
see [CODESTYLE](https://github.com/irov/Mengine/wiki/Codestyle) for more information.

Code Guards
-------
[![CodeFactor](https://www.codefactor.io/repository/github/irov/mengine/badge)](https://www.codefactor.io/repository/github/irov/mengine)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/830296ccc09541d4a03d8c3753f25dc4)](https://www.codacy.com/app/irov13/Mengine?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=irov/Mengine&amp;utm_campaign=Badge_Grade)

License
-------
see [LICENSE](https://raw.githubusercontent.com/irov/Mengine/master/LICENSE) for more information.
