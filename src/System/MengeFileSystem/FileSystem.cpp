
#	include "FileSystem.h"
#	include "FileManager.h"

#	include "DataStream.h"
#	include "MemoryDataStream.h"
#	include "FileStreamDataStream.h"
#	include "FileStreamOutStream.h"

#	include "shlobj.h"

//#	include <tchar.h>

#	include <Config/Config.h>

#if TARGET_PLATFORM == TP_WINDOWS
#	include <direct.h>
#	include <ShellAPI.h>
#endif

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::FileSystemInterface **_system )
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
void releaseInterfaceSystem( Menge::FileSystemInterface *_system )
{
	delete static_cast<Menge::FileSystem*>( _system );
}

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileSystem::FileSystem()
	{
		m_fileManager = new FileManager(MENGE_TEXT(""));
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
	DataStreamInterface* FileSystem::createMemoryFile( void* _data, std::streamsize _size, bool _freeOnClose )
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
#ifdef MENGE_UNICODE
		int res = _wmkdir( _path.c_str() );
#else
		int res = _mkdir( _path.c_str() );
#endif
		if( !res )
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
#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS
		SHFILEOPSTRUCT fs;
		ZeroMemory(&fs, sizeof(SHFILEOPSTRUCT));
		fs.hwnd = NULL;

#ifdef MENGE_UNICODE
		fs.pFrom = _path.c_str();
#else
		wchar_t lpszW[MAX_PATH];
		String::size_type size = _path.size();
		MultiByteToWideChar(CP_ACP, 0, _path.c_str(), -1, lpszW, size );
		lpszW[_path.size()] = 0;
		lpszW[_path.size()+1] = 0;

		fs.pFrom = lpszW;
#endif

		fs.wFunc = FO_DELETE;
		fs.hwnd = NULL;
		fs.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
		if( ::SHFileOperation( &fs ) != 0 )
		{
			return false;
		}
		return true;
#else
#endif
		/*TCHAR szBuf[80]; 
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
		return false;*/
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::changeDir( const String& _path )
	{
#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS 

#ifdef MENGE_UNICODE
		int res = _wchdir( _path.c_str() );
#else
		int res = _chdir( _path.c_str() );
#endif

		if( !res )
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

#ifdef MENGE_UNICODE
		HRESULT hr;
		TChar szPath[MAX_PATH];

		hr = SHGetFolderPathAndSubDir( NULL,					//hWnd	
			CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,	//csidl
			NULL,										//hToken
			SHGFP_TYPE_CURRENT,							//dwFlags
			_game.c_str(),								//pszSubDir
			szPath);									//pszPath
#else
		/// patch for ansi names
		char *ansistr = NULL;
		String::size_type size = _game.size();
		int length = MultiByteToWideChar(CP_UTF8, 0, _game.c_str(), size, NULL, NULL );
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
		TChar szPath[MAX_PATH];

		hr = SHGetFolderPathAndSubDirA(NULL,					//hWnd	
			CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,	//csidl
			NULL,										//hToken
			SHGFP_TYPE_CURRENT,							//dwFlags
			ansistr,								//pszSubDir
			szPath);									//pszPath

		free( ansistr );
#endif

		if( SUCCEEDED( hr ) )
		{
			m_appDataPath = String( szPath );
		}
		else
		{
			return false;
		}
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
	//////////////////////////////////////////////////////////////////////////
	OutStreamInterface* FileSystem::openOutStream( const Menge::String& _filename, bool _binary )
	{
		String fileName = m_appDataPath + MENGE_TEXT("\\") + _filename;

#ifdef MENGE_UNICODE
		const wchar_t* lpszW = _filename.c_str();
#else
		wchar_t lpszW[MAX_PATH];
		String::size_type size = fileName.size();
		MultiByteToWideChar(CP_ACP, 0, fileName.c_str(), -1, lpszW, size );
		lpszW[fileName.size()] = 0;
#endif

		FileStreamOutStream* outStream = new FileStreamOutStream();
		if( !outStream->open( lpszW, _binary ) )
		{
			delete outStream;
			return NULL;
		}
		return outStream;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystem::closeOutStream( OutStreamInterface* _stream )
	{
		delete static_cast<FileStreamOutStream*>( _stream );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::deleteFile( const String& _filename )
	{
#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS
		SHFILEOPSTRUCT fs;
		ZeroMemory(&fs, sizeof(SHFILEOPSTRUCTW));
		fs.hwnd = NULL;

#ifdef MENGE_UNICODE
		const wchar_t* lpszW = _filename.c_str();
#else
		wchar_t lpszW[MAX_PATH];
		String::size_type size = _filename.size();
		MultiByteToWideChar(CP_ACP, 0, _filename.c_str(), -1, lpszW, size );
		lpszW[_filename.size()] = 0;
		lpszW[_filename.size()+1] = 0;
#endif

		fs.pFrom = lpszW;
		fs.wFunc = FO_DELETE;
		fs.hwnd = NULL;
		fs.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
		if( ::SHFileOperation( &fs ) != 0 )
		{
			return false;
		}
		return true;
#else
#endif
	}
	//////////////////////////////////////////////////////////////////////////
}