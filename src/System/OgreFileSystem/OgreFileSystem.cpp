#	include "OgreFileSystem.h"
#	include "OgreFileData.h"

#	include "shlobj.h"

#	include <tchar.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#	include <direct.h>
#	include <ShellAPI.h>
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
	/// !!! FIXME !!!! ///
	Ogre::Root* root = new Ogre::Root("", "", "Menge.log" );
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
	OgreFileData * fileData = 0;

	if( m_arch->exists( _filename ) == false )
	{
		printf( "OgreFileSystem::openFile: %s is not exists\n", _filename );
		return 0;
	}

	try
	{
		Ogre::DataStreamPtr data = m_arch->open( _filename );

		if( data.isNull() )
		{
			printf( "OgreFileSystem::openFile: %s data is null\n", _filename );
			return fileData;
		}

		fileData = new OgreFileData( data );
	}
	catch ( ... )
	{
	}

	return fileData;
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
	/*size_t required_size = mbstowcs( NULL, _path, 0 ) + 1; 

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
	}*/
	if( !_mkdir( _path ) )
	{
		return true;
	}

#else
	assert(!"Not released yet!");
#endif

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool OgreFileSystem::deleteFolder( const char* _path )
{
	// needed for some plugins
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
	SHFILEOPSTRUCTA fs;
	ZeroMemory(&fs, sizeof(SHFILEOPSTRUCTA));
	fs.hwnd = NULL;

	//char* dir = new char[len + 2];
	char dir[MAX_PATH];
	::GetCurrentDirectoryA( MAX_PATH, dir );
	strcat( dir, _path );
	unsigned int len = strlen( dir );
	dir[len] = 0;
	dir[len+1] = 0;
	fs.pFrom = dir;
	fs.wFunc = FO_DELETE;
	fs.hwnd = NULL;
	fs.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	if( !::SHFileOperationA( &fs ) )
	{
		return true;
	}
#else
#endif
	TCHAR szBuf[80]; 
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	sprintf(szBuf, "failed with error %d: %s", dw, lpMsgBuf); 

	MessageBox(NULL, szBuf, "Error", MB_OK); 

	LocalFree(lpMsgBuf);
	return false;
}
//////////////////////////////////////////////////////////////////////////
bool OgreFileSystem::changeDir( const char* _path )
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
	if( !_chdir( _path ) )
	{
		return true;
	}
#else
#endif
	return false;
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::addResourceLocation( const char* _path )
{
	char dir[MAX_PATH];
	::GetCurrentDirectoryA( MAX_PATH, dir );

	Ogre::ResourceGroupManager * resourceGroupMgr = 
		Ogre::ResourceGroupManager::getSingletonPtr();

	resourceGroupMgr->addResourceLocation( Ogre::String(dir) + Ogre::String(_path), "FileSystem", "Default", false );
}
//////////////////////////////////////////////////////////////////////////
void OgreFileSystem::initResources()
{
	Ogre::ResourceGroupManager * resourceGroupMgr = 
		Ogre::ResourceGroupManager::getSingletonPtr();

	resourceGroupMgr->initialiseResourceGroup("Default");
}