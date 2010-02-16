-------------------------------------------------------------------------------
NOW:

python 
[2.5.2]
http://www.python.org/ftp/python/2.5.2/Python-2.5.2.tar.bz2

pybind
[head]
https://pybind.svn.sourceforge.net/svnroot/pybind

expat
[2.0.1]
http://dfn.dl.sourceforge.net/sourceforge/expat/expat-2.0.1.tar.gz

OIS
[1.2]
http://sourceforge.net/projects/wgois
http://dfn.dl.sourceforge.net/sourceforge/wgois/ois_1.2.0.tar.gz

OpenAL
[1.1 - rev:1471]
svn://connect.creativelabs.com/OpenAL/trunk

pthreads
[2.8.0]
ftp://sourceware.org/pub/pthreads-win32/pthreads-w32-2-8-0-release.tar.gz

ogg
[1.1.3]
http://svn.xiph.org/tags/ogg/libogg-1.1.3

vorbis
[1.2.0]
http://svn.xiph.org/tags/vorbis/libvorbis-1.2.0

theora
[1.0beta3]
http://svn.xiph.org/tags/theora/theora-1.0beta3

Box2D
[2.0.2]
https://box2d.svn.sourceforge.net/svnroot/box2d/tags/Box2D-2.0.2

libPNG
[1.2.29]
http://libpng.sourceforge.net/
http://dfn.dl.sourceforge.net/sourceforge/libpng/lpng1229.zip

libJPEG
[6b]
ftp://ftp.uu.net/graphics/jpeg/jpegsrc.v6b.tar.gz

zlib
[1.2.3]
http://www.zlib.net/
http://www.zlib.net/zlib-1.2.3.tar.gz

CMake
[2.6]
http://www.cmake.org/cmake/resources/software.html
http://www.cmake.org/files/v2.6/cmake-2.6.4-win32-x86.zip

DXSDK8
[DirectX/DX8]
ftp://ftp.qut.edu.au/pub/mirrors/microsoft/directx/directx8/dx81b_sdk.exe
- only includes [DirectX/DX8/include]
- delete DirectX/DX8/include/basetsd.h

GLee
[5.4]
http://elf-stone.com/glee.php

astralax
[magic particles]
http://www.astralax.ru/download/programm/particles_dev.exe
-copy c:\Program Files\Magic Particles (Dev)\SDK\dll

--------------------------------------------------------------------------------
COMMENTS:
-- in Python/PC/pyconfig.h rewrite

#define PY_UNICODE_TYPE wchar_t

-- MinGWPatch - dx8 patch for mingw

--------------------------------------------------------------------------------
