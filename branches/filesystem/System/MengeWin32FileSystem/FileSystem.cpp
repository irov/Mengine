
#	include "FileSystem.h"
#	include "FileStreamOutStream.h"

#	define _WIN32_WINNT 0x0501
#	include <shlobj.h>
#	include <Config/Platform.h>

#	include <direct.h>
#	include <ShellAPI.h>

#	include <sys/stat.h>

//#	include "FileStream.h"
#	include "Win32InputStream.h"

#	include <algorithm>

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
	Win32FileSystem::Win32FileSystem()
	{
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

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::createFolder( const String& _path )
	{
		String uPath = _path;
		correctPath( uPath );
		StringW wPath;
		s_UTF8ToWChar( wPath, uPath );
		BOOL result = ::CreateDirectory( wPath.c_str(), NULL );
		return result == TRUE;
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
		StringW filename_w;
		s_UTF8ToWChar( filename_w, _filename );
		String filename_ansi;
		s_WCharToAnsi( filename_ansi, filename_w );
		correctPath( filename_ansi );
		FileStreamOutStream* outStream = new FileStreamOutStream();
		if( !outStream->open( filename_ansi.c_str() ) )
		{
			delete outStream;
			return NULL;
		}
		return outStream;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileSystem::closeOutputStream( OutputStreamInterface* _stream )
	{
		delete static_cast<FileStreamOutStream*>( _stream );
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
}
