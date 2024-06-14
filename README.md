# Mengine

Mengine is an multiple platforms engine for creating and running graphical games

[![CircleCI](https://circleci.com/gh/irov/Mengine.svg?style=svg)](https://app.circleci.com/pipelines/github/irov/Mengine)

The project uses [CMake](https://cmake.org/) as its build system.

## Windows

To get started, you need to download the source files of the dependency libraries

`Mengine\build\downloads\downloads.bat`
    
Next, we can build, for example, a debug for [Visual Studio 17 2022](https://visualstudio.microsoft.com/downloads/)

`Mengine\build\msvc17\build_depends_msvc17_debug.bat`

And generate the solution

`Mengine\build\msvc17\make_solution_msvc17_debug.bat`
    
Which we can find in the folder

`Mengine\solutions\solution_msvc17_debug\Mengine_WIN32.sln`
    
Here, we will find the main project `Win32Application`

Good luck with the build and usage!

-------

To get [cppcheck](http://cppcheck.sourceforge.net/) working, you need to add the following to .git/config

```
[include]
	path = ../.gitconfig
```

## Projects

Games that are being developed or have been released on this engine

Mobile
-------
[Numberphile](https://www.wonderland-games.com/projects/numberphile)

[![Android](https://raw.githubusercontent.com/irov/badgets/master/half/GetItOnGooglePlay_Badge_Web_color_English.png)](https://play.google.com/store/apps/details?id=com.wonderland.numberphile)
[![Apple](https://raw.githubusercontent.com/irov/badgets/master/half/Download_on_the_App_Store_Badge_US-UK_RGB.png)](https://apps.apple.com/app/id6498917681)

Welcome to the world of Numberphile, a thrilling attention-training game that will put your numerical skills to the ultimate test! Designed for speed and concentration, this game is a haven for those who revel in the beauty of numbers and the excitement of a challenge.

[Evil Monkey](https://www.wonderland-games.com/projects/evil-monkey)

[![Android](https://raw.githubusercontent.com/irov/badgets/master/half/GetItOnGooglePlay_Badge_Web_color_English.png)](https://play.google.com/store/apps/details?id=org.Wonderland.EvilMonkey) 

There’s a very hungry and very Evil Monkey under the banana tree! Try to steal those sweet, sweet bananas without drawing the monkey’s wrath. Shake bananas from the tree and if you get too greedy – you’re in big trouble!

[The Dungeon Chronicles](https://www.wonderland-games.com/projects/the-dungeon-chronicles)

[![Android](https://raw.githubusercontent.com/irov/badgets/master/half/GetItOnGooglePlay_Badge_Web_color_English.png)](https://www.wonderland-games.com/projects/the-dungeon-chronicles)

Fight the creatures of darkness with their own weapons to make your way through the gloomy dungeons and save princess from the Skeleton King!

Desktop
-------

[Christmas Adventure: Candy Storm](https://store.steampowered.com/app/456430/Christmas_Adventure_Candy_Storm) Lisa’s grandfather, Santa Claus, has asked for her help delivering Christmas presents to a remote town. Trying to shorten her way, she drives through an old forest when suddenly her car turns into candies!! Moreover, everything around is scattered with candies and covered with other sweets! Help Lisa investigate what’s happened and help fix her car so she can make it to town in time to save Christmas!

[Christmas Adventures: A Winter Night's Dream](https://store.steampowered.com/app/1490790/Christmas_Adventures_A_Winter_Nights_Dream) Shiny snowflakes, jingling bells and magic in the air… All this might not happen, because dark forces want to destroy our Christmas celebrations… again. We need your help!

[The Wisbey Mystery](https://store.steampowered.com/app/575960/The_Wisbey_Mystery) For many years locals have avoided the manor by the woodside. Its dark and dilapidated walls keep a terrible secret about the unsolved murder of the sheriff and his wife. The mystery continues when a young journalist, writing a story about the incident, disappears under murky circumstances. Maybe the maniac’s diary he found earlier has something to do with it… Solve the manor’s mystery and save the journalist before it’s too late!

[Camelot 2: The Holy Grail](https://www.bigfishgames.com/us/en/games/18632/camelot-2-the-holy-grail-ce) When Guinevere, the queen of Camelot, is stricken with a mysterious illness, King Arthur turns to the court magician to find a cure. It's soon discovered that the healing power of the legendary Holy Grail may be the only way to save the queen's life, but the Grail has been lost somewhere in the vast expanses of Avalon, a dangerous realm full of mystery. As the king's trusted magician, it's up to you to make your way into Avalon, locate the Grail, and return to the queen before it's too late! Seize this chance to be the kingdom's hero in this legendary Hidden-Object adventure!

[Camelot: Wrath of the Green Knight](https://www.bigfishgames.com/us/en/games/18535/camelot-wrath-of-the-green-knight-ce) Dark clouds hover over Camelot. The Lady of the Lake is missing, and her husband, the Green Knight, blames King Arthur, which threatens to ruin the peace that Arthur’s court has worked so hard to achieve. As Merlin’s apprentice, your job is to prove your King’s innocence and perhaps find the real culprit behind this dastardly kidnapping. It’s time to prove yourself as a prospective court mage by finding the traitor who plotted to bring down the King and preventing all-out war! Step into a world of nobility and intrigue in this exciting new Hidden-Object Puzzle Adventure game!

[Pride and Prejudice: Blood Ties](https://www.bigfishgames.com/us/en/games/14825/pride-and-prejudice-blood-ties-ce) Recently some odd, dark rumors surround Rosings Park, Lady Catherine’s grand estate. But things get serious when you get a letter from your beloved Mr. Darcy saying that your younger sister Lydia is missing and was last seen at Rosings! What’s more, your beloved has broken up with you and forbids you to visit! Are you strong enough to fight for your love and your missing sister? You’ll need courage and heart to save the day in this eerie version of the classic love story turned Hidden-Object Puzzle Adventure!

[Amulet of Dreams](https://store.steampowered.com/app/476920/Amulet_of_Dreams) Alison’s sudden illness was a shock to everyone. Doctors do not understand what was happening to her, and what caused her illness. Family and doctors are desperate, and only faithful friend never lost hope. Faithful friendfriend. An ordinary little boy named Aiden. Aiden – it’s you.

[The Cursed Island: Mask of Baragus](https://www.bigfishgames.com/games/8368/the-cursed-island-mask-of-baragus) Overcoming considerable obstacles Casey arrives on the island to discover, not only has her father disappeared but his entire team has also gone missing! Word quickly spreads that Stromberg, one of the team members, attempted to invoke the spirit Baragus using a mysterious and ancient artifact.

[Into the Haze](https://www.bigfishgames.com/games/7727/into-the-haze) Rescue a young girl from the clutches of an ancient demon in Into the Haze! Guide your character through the mist of a hair-raising supernatural adventure, where she'll encounter an ancient fog demon summoned by her sister-in-law in a magic ritual gone wrong.

[1001 Nights: The Adventures of Sindbad](https://www.bigfishgames.com/games/5394/1001-nights-the-adventures-of-sindbad) Help the lovely Princess Chalida locate the seven gems of her royal crown and break the curse that has trapped her in a body of wood!

[Marie Antoinette and the Disciples of Loki](https://www.bigfishgames.com/games/6978/marie-antoinette-and-the-disciples-of-loki) The year is 1787 and the French monarchy is living its last years of peace. In Vienna, Mozart is just about to finish his latest opera. Far to the north, in Iceland, a tremendous volcano eruption has already changed the face of Europe forever. What is the link between those events?

## Core Team

These are the humans that form the Mengine core team, which runs the project.

| [@irov][irov-user] |
| :---: |
| [![][irov-img]][irov-user] |

[irov-user]: https://github.com/irov
[irov-img]: https://avatars.githubusercontent.com/u/1975501?s=100&v=4

Profiler
-------
see [OPTICK](https://github.com/bombomby/optick)

Code Style
-------
see [CODESTYLE](https://github.com/irov/Mengine/wiki/Codestyle) for more information.

Code Guards
-------
[![CodeFactor](https://www.codefactor.io/repository/github/irov/mengine/badge)](https://www.codefactor.io/repository/github/irov/mengine)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/830296ccc09541d4a03d8c3753f25dc4)](https://www.codacy.com/app/irov13/Mengine?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=irov/Mengine&amp;utm_campaign=Badge_Grade)
[![CodeQL](https://github.com/irov/Mengine/actions/workflows/codeql-analysis.yml/badge.svg?branch=master)](https://github.com/irov/Mengine/actions/workflows/codeql-analysis.yml)

License
-------
see [LICENSE](https://raw.githubusercontent.com/irov/Mengine/master/LICENSE) for more information.
