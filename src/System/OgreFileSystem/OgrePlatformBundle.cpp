#	include "OgrePlatformBundle.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE

#	include <CoreFoundation/CoreFoundation.h>
Ogre::String platformBundlePath( const Ogre::String & _path )
{
	char path[1024];
	CFBundleRef mainBundle = CFBundleGetMainBundle();    
	assert(mainBundle);

	CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);    
	assert(mainBundleURL);

	CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);    
	assert(cfStringRef);

	CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);
	CFRelease(mainBundleURL);
	CFRelease(cfStringRef);

	return Ogre::String(path) + '/' + _path;
}

#else

Ogre::String platformBundlePath( const Ogre::String & _path )
{
	return _path;
}

#endif