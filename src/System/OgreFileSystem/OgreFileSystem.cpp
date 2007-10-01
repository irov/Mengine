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
{
	m_resourceMgr = Ogre::ResourceGroupManager::getSingletonPtr();
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::loadPath( const char * _path )
{
	m_resourceMgr->addResourceLocation( platformBundlePath( _path ), "FileSystem" );
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::loadPak(const char * _pak )
{
	m_resourceMgr->addResourceLocation( platformBundlePath( _pak ), "Zip" );
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::unloadPak( const char * _pak )
{
	m_resourceMgr->removeResourceLocation( platformBundlePath( _pak ) );
}
//////////////////////////////////////////////////////////////////////////
FileDataInterface *	OgreFileSystem::openFile(const char *	_filename)
{
	Ogre::DataStreamPtr data = m_resourceMgr->openResource( _filename );

	if( data.isNull() )
	{
		return 0;
	}

	return new OgreFileData( data.getPointer() );
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::closeFile( FileDataInterface * _fd )
{
	delete static_cast<OgreFileData*>(_fd);
}
//////////////////////////////////////////////////////////////////////////
bool OgreFileSystem::existFile(const char * _filename)
{
	const Ogre::String & group = m_resourceMgr->findGroupContainingResource( _filename );

	return group.empty() == false;
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
