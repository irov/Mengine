#	include "OgreFileSystem.h"
#	include "OgreFileData.h"

#	include "shlobj.h"

#	include <tchar.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#	include <direct.h>
#endif

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( FileSystemInterface **_system )
{
	try
	{
		*_system = new OgreFileSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( FileSystemInterface *_system )
{
	delete static_cast<OgreFileSystem*>( _system );
}
//////////////////////////////////////////////////////////////////////////
OgreFileSystem::OgreFileSystem()
: m_arch(0)
{
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::loadPath( const char * _path )
{
	m_arch = Ogre::ArchiveManager::getSingleton().load( _path, "FileSystem" );
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::loadPak( const char * _pak )
{
	Ogre::ArchiveManager::getSingleton().load( _pak, "Zip" );
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::unloadPak( const char * _pak )
{
	Ogre::ArchiveManager::getSingleton().unload( _pak );
}
//////////////////////////////////////////////////////////////////////////
FileDataInterface *	OgreFileSystem::openFile( const char * _filename )
{
	printf( "OgreFileSystem::openFile: %s\n", _filename );

	try
	{
		Ogre::DataStreamPtr data = m_arch->open( _filename );

		if( data.isNull() )
		{
			return 0;
		}

		return new OgreFileData( data );
	}
	catch ( ... )
	{
		return 0;
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::closeFile( FileDataInterface * _fd )
{
	delete static_cast<OgreFileData*>(_fd);
}
//////////////////////////////////////////////////////////////////////////
bool OgreFileSystem::existFile( const char * _filename )
{
	return m_arch->exists( _filename );
}
//////////////////////////////////////////////////////////////////////////
const char * OgreFileSystem::platformBundlePath()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#	include <CoreFoundation/CoreFoundation.h>
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

	return _path;
#else
	return "";
#endif
}
//////////////////////////////////////////////////////////////////////////
bool OgreFileSystem::createFolder( const char * _path )
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#
	size_t required_size = mbstowcs( NULL, _path, 0 ) + 1; 

	std::vector<wchar_t> convpath( required_size );

    size_t conv_res = mbstowcs( &convpath[0], _path, required_size );

	if( conv_res <= 0 )
	{
		assert(!"conversion from char to wchar_t failed!");
		return false;
	}
  
	int res = SHCreateDirectoryEx( NULL, &convpath[0], NULL );

	if ( res == ERROR_SUCCESS || res == ERROR_FILE_EXISTS || res == ERROR_ALREADY_EXISTS )
	{
		return true;
	}
#else
	assert(!"Not released yet!");
#endif

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool OgreFileSystem::changeDir( const char* _path )
{
	// needed for some plugins
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation( _path, "FileSystem", "Default" );
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
	if( !_chdir( _path ) )
	{
		return true;
	}
#else
#endif
	return false;
}
