@echo off

@set toolsdir=%~d0%~p0\..\dependencies

md dependencies

%toolsdir%\wget\wget.exe --no-clobber http://stahlworks.com/dev/unzip.exe
%toolsdir%\wget\wget.exe --no-clobber --no-check-certificate https://github.com/arf-it/marmalade-libvorbis/archive/master.zip -O dependencies/marmalade-libvorbis-master.zip
%toolsdir%\wget\wget.exe --no-clobber --no-check-certificate https://github.com/madmaw/marmalade-openal/archive/master.zip -O dependencies/marmalade-openal-master.zip

unzip -o dependencies/marmalade-libvorbis-master.zip -d dependencies
unzip -o dependencies/marmalade-openal-master.zip -d dependencies

pause