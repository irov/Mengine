
#	include "FileSystem.h"
#	include "FileData.h"
#	include "FileManager.h"

#	include "shlobj.h"

#	include <tchar.h>

#	include <Config/Config.h>

#if TARGET_PLATFORM == TP_WINDOWS
#	include <direct.h>
#	include <ShellAPI.h>
#endif

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( FileSystemInterface **_system )
{
	try
	{
		*_system = new MengeFileSystem();
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
	delete static_cast<MengeFileSystem*>( _system );
}
//////////////////////////////////////////////////////////////////////////
MengeFileSystem::MengeFileSystem()
{
	m_fileManager = new FileManager("");
}
//////////////////////////////////////////////////////////////////////////
MengeFileSystem::~MengeFileSystem()
{
	delete m_fileManager;
}
//////////////////////////////////////////////////////////////////////////
void MengeFileSystem::loadPath( const char * _path )
{
	//m_arch = Ogre::ArchiveManager::getSingleton().load( _path, "FileSystem" );
	m_fileManager->setInitPath( _path );
}
//////////////////////////////////////////////////////////////////////////
void MengeFileSystem::loadPak( const char * _pak )
{
	//Ogre::ArchiveManager::getSingleton().load( _pak, "Zip" );
}
//////////////////////////////////////////////////////////////////////////
void MengeFileSystem::unloadPak( const char * _pak )
{
	//Ogre::ArchiveManager::getSingleton().unload( _pak );
}
//////////////////////////////////////////////////////////////////////////
FileDataInterface *	MengeFileSystem::openFile( const char * _filename )
{
	MengeFileData * fileData = 0;

	printf("OgreFileSystem: opening file %s\n", _filename );
	if( m_fileManager->exists( _filename ) == false )
	{
		printf( "OgreFileSystem::openFile: %s is not exists\n", _filename );
		return 0;
	}

	try
	{
		DataStream* data = m_fileManager->open( _filename );

		if( !data )
		{
			printf( "OgreFileSystem::openFile: %s data is null\n", _filename );
			return fileData;
		}

		fileData = new MengeFileData( data );
	}
	catch ( ... )
	{
	}

	return fileData;
}
//////////////////////////////////////////////////////////////////////////
void MengeFileSystem::closeFile( FileDataInterface * _fd )
{
	delete static_cast<MengeFileData*>(_fd);
}
//////////////////////////////////////////////////////////////////////////
bool MengeFileSystem::existFile( const char * _filename )
{
	return m_fileManager->exists( _filename );
}
//////////////////////////////////////////////////////////////////////////
const char * MengeFileSystem::platformBundlePath()
{
#if OTARGET_PLATFORM == TP_APPLE
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
bool MengeFileSystem::createFolder( const char * _path )
{
#if TARGET_PLATFORM == TP_WINDOWS 
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
bool MengeFileSystem::deleteFolder( const char* _path )
{
	// needed for some plugins
#if TARGET_PLATFORM == TP_WINDOWS 
	SHFILEOPSTRUCTA fs;
	ZeroMemory(&fs, sizeof(SHFILEOPSTRUCTA));
	fs.hwnd = NULL;

	//char* dir = new char[len + 2];
	char dir[MAX_PATH];
	::GetCurrentDirectoryA( MAX_PATH, dir );
	strcat_s( dir, _path );
	std::size_t len = strlen( dir );
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

	sprintf_s(szBuf, "failed with error %d: %s", dw, lpMsgBuf); 

	MessageBox(NULL, szBuf, "Error", MB_OK); 

	LocalFree(lpMsgBuf);
	return false;
}
//////////////////////////////////////////////////////////////////////////
bool MengeFileSystem::changeDir( const char* _path )
{
#if TARGET_PLATFORM == TP_WINDOWS 
	if( !_chdir( _path ) )
	{
		return true;
	}
#else
#endif
	return false;
}
//////////////////////////////////////////////////////////////////////////
const TCHAR* MengeFileSystem::getApplicationDataPath( const TCHAR* _game )
{
#if TARGET_PLATFORM == TP_WINDOWS 
	static TCHAR path[MAX_PATH];
	::SHGetFolderPathAndSubDir(NULL,
								CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
								NULL,
								SHGFP_TYPE_CURRENT,
								_game,
								path);
	return path;
	//	TEXT()
#endif
	return NULL;
}