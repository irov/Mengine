
#	include "FileSystem.h"
#	include "FileManager.h"

#	include "DataStream.h"
#	include "MemoryDataStream.h"
#	include "FileStreamDataStream.h"

#	include "shlobj.h"

//#	include <tchar.h>

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
		*_system = new Menge::FileSystem();
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
	delete static_cast<Menge::FileSystem*>( _system );
}

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileSystem::FileSystem()
	{
		m_fileManager = new FileManager("");
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystem::~FileSystem()
	{
		delete m_fileManager;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystem::loadPath( const String& _path )
	{
		//m_arch = Ogre::ArchiveManager::getSingleton().load( _path, "FileSystem" );
		m_fileManager->setInitPath( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystem::loadPak( const String& _pak )
	{
		//Ogre::ArchiveManager::getSingleton().load( _pak, "Zip" );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystem::unloadPak( const String& _pak )
	{
		//Ogre::ArchiveManager::getSingleton().unload( _pak );
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* FileSystem::openFile( const String& _filename )
	{
		DataStream* fileData = 0;

		printf("MengeFileSystem: opening file %s\n", _filename.c_str() );
		if( m_fileManager->exists( _filename ) == false )
		{
			printf( "MengeFileSystem::openFile: %s is not exists\n", _filename.c_str() );
			return 0;
		}

		try
		{
			fileData = m_fileManager->open( _filename );

			if( !fileData )
			{
				printf( "MengeFileSystem::openFile: %s data is null\n", _filename.c_str() );
				return fileData;
			}
		}
		catch ( ... )
		{
		}

		return fileData;
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* FileSystem::createMemoryFile( void* _data, std::size_t _size, bool _freeOnClose )
	{
		return static_cast<DataStreamInterface*>( new MemoryDataStream( _data, _size, _freeOnClose ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystem::closeStream( DataStreamInterface* _stream )
	{
		if( _stream->isMemory() )
		{
			delete static_cast<MemoryDataStream*>(_stream);
		}
		else
		{
			delete static_cast<FileStreamDataStream*>(_stream);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::existFile( const String& _filename )
	{
		return m_fileManager->exists( _filename );
	}
	//////////////////////////////////////////////////////////////////////////
	const char * FileSystem::platformBundlePath()
	{
#if MENGE_PLATFORM == MENGE_PLATFORM_APPLE
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
	bool FileSystem::createFolder( const String& _path )
	{
#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS 
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
		if( !_mkdir( _path.c_str() ) )
		{
			return true;
		}

#else
		assert(!"Not released yet!");
#endif

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::deleteFolder( const String& _path )
	{
		// needed for some plugins
#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS
		SHFILEOPSTRUCTA fs;
		ZeroMemory(&fs, sizeof(SHFILEOPSTRUCTA));
		fs.hwnd = NULL;

		//char* dir = new char[len + 2];
		char dir[MAX_PATH];
		::GetCurrentDirectoryA( MAX_PATH, dir );
		strcat_s( dir, _path.c_str() );
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
	bool FileSystem::changeDir( const String& _path )
	{
#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS 
		if( !_chdir( _path.c_str() ) )
		{
			return true;
		}
#else
#endif
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::initAppDataPath( const String& _game )
	{
#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS

		/// patch for ansi names
		char *ansistr = NULL;
		size_t length = MultiByteToWideChar(CP_UTF8, 0, _game.c_str(), _game.length(), NULL, NULL );
		WCHAR *lpszW = NULL;

		lpszW = new WCHAR[length+1];
		ansistr = ( char * ) calloc ( sizeof(char), length+5 );

		//this step intended only to use WideCharToMultiByte
		MultiByteToWideChar(CP_UTF8, 0, _game.c_str(), -1, lpszW, length );

		//Conversion to ANSI (CP_ACP)
		WideCharToMultiByte(CP_ACP, 0, lpszW, -1, ansistr, length, NULL, NULL);

		ansistr[length] = 0;

		delete[] lpszW;
		////

		HRESULT hr;
		TCHAR szPath[MAX_PATH];

		hr = SHGetFolderPathAndSubDir(NULL,					//hWnd	
			CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,	//csidl
			NULL,										//hToken
			SHGFP_TYPE_CURRENT,							//dwFlags
			ansistr,								//pszSubDir
			szPath);									//pszPath

		if( SUCCEEDED( hr ) )
		{
			m_appDataPath = String( szPath );
		}
		else
		{
			return false;
		}
		free( ansistr );
		return true;
#else
		return false;
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	const Menge::String& FileSystem::getAppDataPath()
	{
		return m_appDataPath;
	}
}