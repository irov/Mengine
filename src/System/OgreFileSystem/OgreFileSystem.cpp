#	include "OgreFileSystem.h"
#	include "OgreFileData.h"

#	include "OgrePlatformBundle.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem(FileSystemInterface **_system)
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
void releaseInterfaceSystem(FileSystemInterface *_system)
{
	delete static_cast<OgreFileSystem*>(_system);
}
//////////////////////////////////////////////////////////////////////////
OgreFileSystem::OgreFileSystem()
: m_resourceMgr(0)
{
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::init( Ogre::ResourceGroupManager * _resourceMgr )
{
	m_resourceMgr = _resourceMgr;
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::loadPath( const char * _path )
{
	m_resourceMgr
		->addResourceLocation( _path, "FileSystem", "OgreFileSystem", true );
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::loadPak(const char * _pak )
{
	m_resourceMgr
		->addResourceLocation( _pak, "Zip", "OgreFileSystem", true );
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::unloadPak( const char * _pak )
{
	m_resourceMgr
		->removeResourceLocation( _pak, "OgreFileSystem" );
}
//////////////////////////////////////////////////////////////////////////
FileDataInterface *	OgreFileSystem::openFile(const char *	_filename)
{
	try
	{
		Ogre::DataStreamPtr data = m_resourceMgr
			->openResource( _filename, "OgreFileSystem" );


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
bool OgreFileSystem::existFile(const char * _filename)
{
	try
	{
		const Ogre::String & group = m_resourceMgr
			->findGroupContainingResource( _filename );

		return group.empty() == false;
	}
	catch ( ... )
	{
		return false;
	}	
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
