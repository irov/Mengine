#	include "Interface/LogSystemInterface.h"

#	include "FileSystem.h"

#	include "DataStream.h"
#	include "MemoryDataStream.h"
#	include "FileStreamDataStream.h"
#	include "FileStreamOutStream.h"

#	include "shlobj.h"
#	include <Config/Config.h>

#	include <direct.h>
#	include <ShellAPI.h>

#	include "Menge/Utils.h"

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + StringA("\n"), LM_LOG );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + StringA("\n"), LM_ERROR );
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
	static bool s_isAbsolutePath( const String& _path )
	{
		if ( /*::isalpha( unsigned char( _path[0] ) ) && */_path[1] == ':' )
		{
			return true;
		}
		return _path[0] == '/' || _path[0] == '\\';
	}
	//////////////////////////////////////////////////////////////////////////
	static String s_concatenatePath( const String& _base, const String& _name )
	{
		if ( _base.empty() || s_isAbsolutePath( _name.c_str() ) )
		{
			return _name;
		}
		else
		{
			return _base + '/' + _name;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystem::FileSystem()
		: m_logSystem( 0 )
		, m_logStream( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystem::~FileSystem()
	{
		if( m_logSystem != NULL && m_logStream != NULL )
		{
			m_logSystem->unregisterLogger( m_logStream );
			closeOutStream( m_logStream );
			m_logStream = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystem::loadPath( const String& _path )
	{
		m_initPath = _path;
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

		LOG( "Opening file: \"" + _filename + "\"" );
		
		if( existFile( _filename ) == false )
		{
			LOG_ERROR( "file \"" + _filename + "\" does not exist" );
			return 0;
		}

		try
		{
			String full_path = s_concatenatePath( m_initPath, _filename );
			StringW full_path_w = Utils::AToW( full_path );
			// Use filesystem to determine size 
			// (quicker than streaming to the end and back)
			struct _stat tagStat;

			int ret = _wstat( full_path_w.c_str(), &tagStat );

			std::ifstream *origStream = new std::ifstream();

			// Always open in binary mode
			origStream->open( full_path.c_str(), std::ios::in | std::ios::binary );

			// Should check ensure open succeeded, in case fail for some reason.
			if ( origStream->fail() )
			{
				delete origStream;
			}

			/// Construct return stream, tell it to delete on destroy
			FileStreamDataStream* stream = 
				new FileStreamDataStream( origStream, tagStat.st_size, true );

			fileData = static_cast<DataStream*>(stream);

			if( !fileData )
			{
				LOG_ERROR( "unrecognized error while opening file \"" + _filename + "\"" );
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
		delete _stream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::existFile( const String& _filename )
	{
		String full_path = s_concatenatePath( m_initPath, _filename );
		StringW full_path_w = Utils::AToW( full_path );

		struct _stat tagStat;
		bool ret = ( _wstat( full_path_w.c_str(), &tagStat ) == 0 );
		return ret;
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
		StringW path_w = Utils::AToW( _path );
		int res = _wmkdir( path_w.c_str() );
		if( !res )
		{
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::deleteFolder( const String& _path )
	{
		StringW path_w = Utils::AToW( _path );

		SHFILEOPSTRUCT fs;
		ZeroMemory(&fs, sizeof(SHFILEOPSTRUCT));
		fs.hwnd = NULL;

		Menge::TCharW path[MAX_PATH];
		wcscpy( path, path_w.c_str() );
		path[ _path.size() + 1 ] = 0;
		fs.pFrom = path;

		fs.wFunc = FO_DELETE;
		fs.hwnd = NULL;
		fs.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
		if( ::SHFileOperation( &fs ) != 0 )
		{
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

			//sprintf_s(szBuf, "failed with error %d: %s", dw, lpMsgBuf); 

			//MessageBox(NULL, szBuf, "Error", MB_OK); 

			LocalFree(lpMsgBuf);*/
			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::initAppDataPath( const String& _game )
	{
		StringW game_w = Utils::AToW( _game );

		HRESULT hr;
		TCharW szPath[MAX_PATH];

		hr = SHGetFolderPathAndSubDir( NULL,					//hWnd	
			CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,	//csidl
			NULL,										//hToken
			SHGFP_TYPE_CURRENT,							//dwFlags
			game_w.c_str(),								//pszSubDir
			szPath);									//pszPath

		if( SUCCEEDED( hr ) )
		{
			m_appDataPath = Utils::WToA( szPath );
		}
		else
		{
			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& FileSystem::getAppDataPath()
	{
		return m_appDataPath;
	}
	//////////////////////////////////////////////////////////////////////////
	OutStreamInterface* FileSystem::openOutStream( const String& _filename, bool _binary )
	{
		String filename;
		if( m_appDataPath.empty() )
		{
			filename = _filename;
		}
		else
		{
			filename = m_appDataPath + "\\" + _filename;
		}
		//StringW filename_w = Utils::AToW( filename );

		FileStreamOutStream* outStream = new FileStreamOutStream();
		if( !outStream->open( filename.c_str(), _binary ) )
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
		SHFILEOPSTRUCT fs;
		ZeroMemory(&fs, sizeof(SHFILEOPSTRUCTW));
		fs.hwnd = NULL;

		StringW filename_w = Utils::AToW( _filename );
		const Menge::TCharW* lpszW = filename_w.c_str();

		fs.pFrom = lpszW;
		fs.wFunc = FO_DELETE;
		fs.hwnd = NULL;
		fs.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
		if( ::SHFileOperation( &fs ) != 0 )
		{
			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::inititalize( LogSystemInterface* _logSystemInterface )
	{
		m_logSystem = _logSystemInterface;
		if( m_logSystem != NULL )
		{
			m_logStream = openOutStream( "Menge.log", false );
			if( m_logStream != NULL )
			{
				m_logSystem->registerLogger( m_logStream );
				LOG( "Starting log to Menge.log" );
			}
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
