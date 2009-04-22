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
	static Menge::StringW s_UTF8ToWChar( const Menge::String& _utf8 )
	{
		int size = MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, 0, 0 );
		wchar_t* conv = new wchar_t[size];
		MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, conv, size );
		Menge::StringW out( conv );
		delete[] conv;
		return out;
	}
	//////////////////////////////////////////////////////////////////////////
	static Menge::String s_WCharToUTF8( const Menge::StringW& _widechar )
	{
		int size = WideCharToMultiByte( CP_UTF8, 0, _widechar.c_str(), -1, NULL, 0, NULL, NULL );
		char* conv = new char[size];
		WideCharToMultiByte( CP_UTF8, 0, _widechar.c_str(), -1, conv, size, NULL, NULL );
		Menge::String out( conv );
		delete[] conv;
		return out;
	}
	//////////////////////////////////////////////////////////////////////////
	static Menge::String s_WCharToAnsi( const Menge::StringW& _widechar )
	{
		int size = WideCharToMultiByte( CP_ACP, 0, _widechar.c_str(), -1, NULL, 0, NULL, NULL );
		char* conv = new char[size];
		WideCharToMultiByte( CP_ACP, 0, _widechar.c_str(), -1, conv, size, NULL, NULL );
		Menge::String out( conv );
		delete[] conv;
		return out;
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystem::FileSystem()
		: m_logSystem( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileSystem::~FileSystem()
	{
		for( std::vector<FileStream*>::iterator it = m_fileStreamPool.begin(), it_end = m_fileStreamPool.end()
			; it != it_end
			; it++ )
		{
			static StringW dummy = L"";
			FileStream* fileStream = (*it);
			fileStream = new (fileStream) FileStream( dummy, false );
			delete fileStream;
		}
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
	void FileSystem::loadPath( const String& _path )
	{
		m_initPath = makeCorrectPath( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* FileSystem::openFile( const String& _filename, bool _map/* = false */ )
	{
		DataStreamInterface* fileData = 0;

		String filenameCorrect = makeCorrectPath( _filename );
		
		String full_path = joinPath( m_initPath, filenameCorrect );
		//StringW full_path_w = Utils::AToW( full_path );
		StringW full_path_w = s_UTF8ToWChar( full_path );

		//FileStream* fileStream = new FileStream( hFile );
		FileStream* fileStream = NULL;
		if( m_fileStreamPool.empty() == true )
		{
			fileStream = new FileStream( full_path_w, _map );
		}
		else
		{
			fileStream = m_fileStreamPool.back();
			fileStream = new (fileStream) FileStream(full_path_w, _map);
			m_fileStreamPool.pop_back();
		}

		if( fileStream->isValid() == false )
		{
			LOG_ERROR( "Error while opening file " + _filename );
			closeStream( fileStream );
			fileStream = NULL;
		}

		return fileStream;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystem::closeStream( DataStreamInterface* _stream )
	{
		FileStream* fileStream = static_cast<FileStream*>( _stream );
		fileStream->~FileStream();
		m_fileStreamPool.push_back( fileStream );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::existFile( const String& _filename )
	{
		String full_path = joinPath( m_initPath, makeCorrectPath( _filename ) );
		//StringW full_path_w = Utils::AToW( full_path );
		StringW full_path_w = s_UTF8ToWChar( full_path );

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
		String realName = s_WCharToUTF8( findData.cFileName );
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
		StringW path_w = s_UTF8ToWChar( _path );
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
		StringW path_w = s_UTF8ToWChar( path_correct );

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
		StringW game_w = s_UTF8ToWChar( path_correct );

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
			m_appDataPath = s_WCharToUTF8( szPath );
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
		String filename_ansi = s_WCharToAnsi( s_UTF8ToWChar( filename ) );

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
		StringW filename_w = s_UTF8ToWChar( path_correct );

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
	Menge::String FileSystem::makeCorrectPath( const String& _path )
	{
		String correctPath = _path;
		std::replace( correctPath.begin(), correctPath.end(), '/', PATH_DELIM );
		return correctPath;
	}
	//////////////////////////////////////////////////////////////////////////
}
