
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
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Win32FileSystem::Win32FileSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32FileSystem::~Win32FileSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputStreamInterface* Win32FileSystem::openInputStream( const String& _filename )
	{
		Win32InputStream* inputStream = new Win32InputStream();

		if( inputStream->open( _filename ) == false )
		{				
			this->closeInputStream( inputStream );
			inputStream = NULL;
		}

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileSystem::closeInputStream( FileInputStreamInterface* _stream )
	{
		Win32InputStream* inputStream = static_cast<Win32InputStream*>( _stream );

		inputStream->close();

		delete inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::existFile( const String& _filename )
	{
		if( _filename.empty() == true )	// current dir
		{
			return true;
		}

		if( _filename.empty() == false 
			&& _filename[_filename.size()-1] == L':' )	// root dir
		{
			return true;	// let it be
		}

		bool found = WindowsLayer::fileExists( _filename );
		return found;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32FileSystem::createFolder( const String& _path )
	{
		bool result = WindowsLayer::createDirectory( _path );

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
		WString path_w;
		WindowsLayer::utf8ToWstr( path_correct, path_w );

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
	FileOutputStreamInterface* Win32FileSystem::openOutputStream( const String& _filename )
	{
		Win32OutputStream* outStream = new Win32OutputStream();
		if( outStream->open( _filename ) == false )
		{
			delete outStream;
			return NULL;
		}

		return outStream;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32FileSystem::closeOutputStream( FileOutputStreamInterface* _stream )
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
		WString filename_w;
		WindowsLayer::utf8ToWstr( path_correct, filename_w );

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
}
