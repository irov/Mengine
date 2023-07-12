download: [msys2](https://www.msys2.org/)

pacman -Syu

[restart]

pacman -Syu
pacman -Su
pacman -S mingw-w64-x86_64-uasm
pacman -S mingw-w64-x86_64-clang
pacman -S mingw-w64-x86_64-lld
pacman -S mingw-w64-x86_64-ninja
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-gdb
pacman -S mingw-w64-x86_64-python-jinja
pacman -S mingw-w64-x86_64-python-requests
