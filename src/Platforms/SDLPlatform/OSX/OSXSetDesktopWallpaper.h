#ifndef OSXSetDesktopWallpaper_H_
#define OSXSetDesktopWallpaper_H_

#include "Config/Config.h"

int OSXGetBuildShortVersionString( char * const _directory );
int OSXGetBuildVersionKey( char * const _directory );
int OSXGetMusicDirectory( char * const _directory );
int OSXGetPicturesDirectory( char * const _directory );
int OSXSetDesktopWallpaper( const char * _url );

#endif
