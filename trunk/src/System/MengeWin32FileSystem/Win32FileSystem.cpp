
#	include "Win32FileSystem.h"

#	include <algorithm>
#	include <cassert>

//#	define _WIN32_WINNT 0x0501
#	include <shlobj.h>

//#	include <direct.h>
#	include <ShellAPI.h>

//#	include "FileStream.h"
#	include "Win32InputStream.h"
#	include "Win32OutputStream.h"


#define  PATH_DELIM '\\'
//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::FileSystemInterface **_system )
{
	try
	{
		*_system = new Menge::Win32FileSystem();
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
	delete static_cast<Menge::Win32FileSystem*>( _system );
}

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static bool s_testForUnicode()
	{
		HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );
		if( hKernel32 != NULL )
		{
			::FreeLibrary( hKernel32 );
			return true;
		}
		return false;
	}
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
	static bool s_findFirstFile( const std::wstring& _path, bool _ansi )
	{
		//HANDLE hFile = INVALID_HANDLE_VALUE;
		DWORD attributes = INVALID_FILE_ATTRIBUTES;
		if( _ansi == true )
		{
			std::string pathAnsi;
			s_WCharToAnsi( pathAnsi, _path );
			//WIN32_FIND_DATAA findData;
			//hFile = FindFirstFileA( pathAnsi.c_str(), &findData );
			attributes = GetFileAttributesA( pathAnsi.c_str() );
		}
		else
		{
			//WIN32_FIND_DATAW findData;
			//HANDLE hFile = FindFirstFileW( _path.c_str(), &findData );
			attributes = GetFileAttributesW( _path.c_str() );
		}
		//if( hFile == INVALID_HANDLE_VALUE )
		if( attributes == INVALID_FILE_ATTRIBUTES )
		{
			DWORD dw = GetLastError();
			return false;
		}
		//FindClose( hFile );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_createDirectory( const std::wstring& _path, bool _ansi )
	{
		BOOL result = FALSE;
		if( _ansi == true )
		{
			std::string pathAnsi;
			s_WCharToAnsi( pathAnsi, _path );
			result = ::CreateDirectoryA( pathAnsi.c_str(), NULL );
		}
		else
		{
			result = ::CreateDirectoryW( _path.c_str(), NULL );
		}
		return result == TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	Win32FileSystem::Win32FileSystem()
		: m_ansiCallsOnly( false )
	{
		m_ansiCallsOnly = (s_testForUnicode() == false);
	}
	//////////////////////////////////////////////////////////////////////////
	Win32FileSystem::~Win32FileSystem()
	{
		for( TInputStreamPool::iterator it = m_inputStreamPool.begin(), it_end = m_inputStreamPool.end();
			it != it_end;
			++it )
		{
			delete (*it);
		}
		m_inputStreamPool.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterface* Win32FileSystem::openInputStream( const String& _filename )
	{
		String filenameCorrect = _filename;
		this->correctPath( filenameCorrect );
		StringW full_path_w;
		s_UTF8ToWChar( full_path_w, filenameCorrect );

		Win32InputStream* inputStream = NULL;

		if( m_inputStreamPool.empty() == true )
		{
			inputStream = new Win32InputStream();
		}
		else
		{
			inputStream = m_inputStreamPool.back();
			m_inputStreamPool.pop_back();
		}

		if( inputStream->open( full_path_w ) == false )
		{				
			this->closeInputStream( inputStream );
			inputStream = NULL;
		}

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileSystem::closeInputStream( InputStreamInterface* _stream )
	{
		Win32InputStream* inputStream = static_cast<Win32InputStream*>( _stream );
		if( inputStream != NULL )
		{
			inputStream->close();
			m_inputStreamPool.push_back( inputStream );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::existFile( const String& _filename )
	{
		if( _filename.empty() == true )	// current dir
		{
			return true;
		}

		String full_path = _filename;
		this->correctPath( full_path );

		StringW full_path_w;
		s_UTF8ToWChar( full_path_w, full_path );

		if( full_path_w.empty() == false 
			&& full_path_w[full_path_w.size()-1] == L':' )	// root dir
		{
			return true;	// let it be
		}

		bool found = s_findFirstFile( full_path_w, m_ansiCallsOnly );
		return found;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::createFolder( const String& _path )
	{
		String uPath = _path;
		correctPath( uPath );
		StringW wPath;
		s_UTF8ToWChar( wPath, uPath );
		bool result = s_createDirectory( wPath, m_ansiCallsOnly );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::deleteFolder( const String& _path )
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
	OutputStreamInterface* Win32FileSystem::openOutputStream( const String& _filename )
	{
		String filenameCorrect = _filename;
		this->correctPath( filenameCorrect );
		StringW full_path_w;
		s_UTF8ToWChar( full_path_w, filenameCorrect );
		
		Win32OutputStream* outStream = new Win32OutputStream();
		if( outStream->open( full_path_w ) == false )
		{
			delete outStream;
			return NULL;
		}
		return outStream;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileSystem::closeOutputStream( OutputStreamInterface* _stream )
	{
		Win32OutputStream* outStream = static_cast<Win32OutputStream*>( _stream );
		if( outStream != NULL )
		{
			outStream->close();
			delete outStream;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::deleteFile( const String& _filename )
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
	void Win32FileSystem::correctPath( String & _path ) const
	{
		std::replace( _path.begin(), _path.end(), '/', PATH_DELIM );
	}
	//////////////////////////////////////////////////////////////////////////
	void* Win32FileSystem::openMappedFile( const String& _filename, int* _size )
	{
		assert( _size );

		String filenameCorrect = _filename;
		this->correctPath( filenameCorrect );
		StringW full_path_w;
		s_UTF8ToWChar( full_path_w, filenameCorrect );

		DWORD shareAttrib = /*m_mapped ? 0 :*/ FILE_SHARE_READ;
		HANDLE hFile = CreateFile( full_path_w.c_str(),    // file to open
									GENERIC_READ,			// open for reading
									shareAttrib,			// share for reading, exclusive for mapping
									NULL,					// default security
									OPEN_EXISTING,			// existing file only
									FILE_ATTRIBUTE_NORMAL,	// normal file
									NULL);					// no attr. template

		if ( hFile == INVALID_HANDLE_VALUE)
		{
			return NULL;
		}

		DWORD fSize = GetFileSize( hFile, NULL );

		if( fSize == INVALID_FILE_SIZE )
		{
			CloseHandle( hFile );
			return NULL;
		}

		HANDLE hMapping = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL );

		if( hMapping == NULL )
		{
			CloseHandle( hFile );
			return NULL;
		}

		void* pMem = MapViewOfFile( hMapping, FILE_MAP_READ, 0, 0, fSize );
		*_size = fSize;

		FileMappingInfo fmInfo = { hFile, hMapping };
		m_fileMappingMap.insert( std::make_pair( pMem, fmInfo ) );

		return pMem;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileSystem::closeMappedFile( void* _file )
	{
		TFileMappingMap::iterator it_find = m_fileMappingMap.find( _file );
		if( it_find != m_fileMappingMap.end() )
		{
			CloseHandle( it_find->second.hMapping );
			CloseHandle( it_find->second.hFile );
			m_fileMappingMap.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
