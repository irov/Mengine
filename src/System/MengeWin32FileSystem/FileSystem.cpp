#	include "Interface/LogSystemInterface.h"

#	include "FileSystem.h"
#	include "FileStreamOutStream.h"

#	define _WIN32_WINNT 0x0501
#	include <shlobj.h>
#	include <Config/Platform.h>

#	include <direct.h>
#	include <ShellAPI.h>

#	include <sys/stat.h>

#	include "FileStream.h"

#	include <algorithm>

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + String("\n"), LM_LOG );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + String("\n"), LM_ERROR );

#define  PATH_DELIM '\\'
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
	static void s_UTF8ToWChar( StringW & _out, const String& _utf8 )
	{
		//int size = MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, 0, 0 );
		wchar_t conv[MAX_PATH];
		MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, conv, MAX_PATH );
		_out = conv;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_WCharToUTF8( String & _out, const StringW& _widechar )
	{
		//int size = WideCharToMultiByte( CP_UTF8, 0, _widechar.c_str(), -1, NULL, 0, NULL, NULL );
		char conv[MAX_PATH];
		WideCharToMultiByte( CP_UTF8, 0, _widechar.c_str(), -1, conv, MAX_PATH, NULL, NULL );
		_out = conv;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_WCharToAnsi( String & _out, const StringW& _widechar )
	{
		//int size = WideCharToMultiByte( CP_ACP, 0, _widechar.c_str(), -1, NULL, 0, NULL, NULL );
		//char* conv = new char[size];
		char conv[MAX_PATH];
		WideCharToMultiByte( CP_ACP, 0, _widechar.c_str(), -1, conv, MAX_PATH, NULL, NULL );
		_out = conv;
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystem::FileSystem()
		: m_logSystem( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystem::~FileSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::isAbsolutePath( const String& _path )
	{
		if ( /*::isalpha( unsigned char( _path[0] ) ) && */_path[1] == ':' )
		{
			return true;
		}
		return _path[0] == '/' || _path[0] == '\\';
	}
	//////////////////////////////////////////////////////////////////////////
	String FileSystem::joinPath( const String& _base, const String& _name )
	{
		if ( _base.empty() || isAbsolutePath( _name.c_str() ) )
		{
			return _name;
		}
		else
		{
			return _base + PATH_DELIM + _name;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* FileSystem::openFile( const String& _filename, bool _map/* = false */ )
	{
		DataStreamInterface* fileData = 0;

		String filenameCorrect = _filename;
		
		this->correctPath( filenameCorrect );
		
		//String full_path = filenameCorrect;
		//StringW full_path_w = Utils::AToW( full_path );
		StringW full_path_w;
		s_UTF8ToWChar( full_path_w, filenameCorrect );

		//FileStream* fileStream = new FileStream( hFile );
		FileStream * fileStream = new FileStream();

		if( fileStream->initialize( full_path_w, _map ) == false )
		{
			LOG_ERROR( "Error while opening file " + _filename );
				
			this->closeStream( fileStream );

			return NULL;
		}

		return fileStream;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystem::closeStream( DataStreamInterface* _stream )
	{
		//FileStream* fileStream = static_cast<FileStream*>( _stream );
		//fileStream->~FileStream();
		//m_fileStreamPool.push_back( fileStream );
		delete static_cast<FileStream*>( _stream );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::existFile( const String& _filename )
	{
		String full_path = _filename;
		this->correctPath( full_path );
		//StringW full_path_w = Utils::AToW( full_path );
		StringW full_path_w;
		s_UTF8ToWChar( full_path_w, full_path );

		//struct _stat tagStat;
		//bool ret = ( _wstat( full_path_w.c_str(), &tagStat ) == 0 );
		//bool ret = ( stat( full_path.c_str(), &tagStat
		WIN32_FIND_DATA findData;
		HANDLE hFile = FindFirstFile( full_path_w.c_str(), &findData );
		if( hFile == INVALID_HANDLE_VALUE )
		{
			return false;
		}

		FindClose( hFile );

#	ifndef MENGE_MASTER_RELEASE
		String realName;
		s_WCharToUTF8( realName, findData.cFileName );

		if( _filename.find( realName ) == String::npos )
		{
			m_logSystem->logMessage( "Warning: filename case mismatch ", LM_ERROR );
			m_logSystem->logMessage( _filename, LM_ERROR );
			m_logSystem->logMessage( " (", LM_ERROR );
			m_logSystem->logMessage( realName, LM_ERROR );
			m_logSystem->logMessage( ")\n", LM_ERROR );
		}
#endif
		//return ret;
		return true;
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
		//StringW path_w = Utils::AToW( _path );
		StringW path_w;
		s_UTF8ToWChar( path_w, _path );
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
		String path_correct = _path;
		String::size_type pos = path_correct.find("/");
		while( pos != String::npos )
		{
			path_correct[pos] = PATH_DELIM;
			pos = path_correct.find("/");
		}
		//StringW path_w = Utils::AToW( _path );
		StringW path_w;
		s_UTF8ToWChar( path_w, path_correct );

		SHFILEOPSTRUCT fs;
		ZeroMemory(&fs, sizeof(SHFILEOPSTRUCT));
		fs.hwnd = NULL;

		Menge::TCharW path[MAX_PATH];
		wcscpy( path, path_w.c_str() );
		path[ path_w.size() + 1 ] = 0;
		fs.pFrom = path;

		fs.wFunc = FO_DELETE;
		fs.hwnd = NULL;
		fs.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
		int err = ::SHFileOperation( &fs );
		if( err != 0 )
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
	bool FileSystem::initAppDataPath( const String & _path, bool _local )
	{
		if( _local == true )
		{
			m_appDataPath = _path;
			return true;
		}

		String path_correct = _path;
		String::size_type pos = path_correct.find("/");
		while( pos != String::npos )
		{
			path_correct[pos] = PATH_DELIM;
			pos = path_correct.find("/");
		}

		//StringW game_w = Utils::AToW( _game );
		StringW game_w;
		s_UTF8ToWChar( game_w, path_correct );

		HRESULT hr;
		TCharW szPath[MAX_PATH];

		hr = SHGetFolderPathAndSubDir( NULL,					//hWnd	
			CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,	//csidl
			NULL,										//hToken
			0,											//dwFlags
			game_w.c_str(),								//pszSubDir
			szPath);									//pszPath

		if( SUCCEEDED( hr ) )
		{
			//m_appDataPath = Utils::WToA( szPath );
			s_WCharToUTF8( m_appDataPath, szPath );
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
		
		if( m_appDataPath.empty() || isAbsolutePath( _filename ) )
		{
			filename = _filename;
		}
		else
		{
			filename = m_appDataPath + PATH_DELIM + _filename;
		}
		//StringW filename_w = Utils::AToW( filename );
		StringW filename_w;
		s_UTF8ToWChar( filename_w, filename );
		String filename_ansi;
		s_WCharToAnsi( filename_ansi, filename_w );

		FileStreamOutStream* outStream = new FileStreamOutStream();
		if( !outStream->open( filename_ansi.c_str(), _binary ) )
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
		String path_correct = _filename;
		String::size_type pos = path_correct.find("/");
		while( pos != String::npos )
		{
			path_correct[pos] = PATH_DELIM;
			pos = path_correct.find("/");
		}

		SHFILEOPSTRUCT fs;
		ZeroMemory(&fs, sizeof(SHFILEOPSTRUCTW));
		fs.hwnd = NULL;

		//StringW filename_w = Utils::AToW( _filename );
		StringW filename_w;
		s_UTF8ToWChar( filename_w, path_correct );

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
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystem::correctPath( String & _path ) const
	{
		std::replace( _path.begin(), _path.end(), '/', PATH_DELIM );
	}
	//////////////////////////////////////////////////////////////////////////
}
