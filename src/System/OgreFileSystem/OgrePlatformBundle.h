#	pragma once

#	include "Ogre.h"

// This function will locate the path to our OgreApplication on OS X,
// unlike windows you can not rely on the curent working directory
// for locating your configuration files and resources.
Ogre::String platformBundlePath( const Ogre::String & _path );