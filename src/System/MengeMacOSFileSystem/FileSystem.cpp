#	include "Interface/LogSystemInterface.h"

#	include "FileSystem.h"

#	include "DataStream.h"
#	include "MemoryDataStream.h"
#	include "FileStreamDataStream.h"
#	include "FileStreamOutStream.h"

#	include "Menge/Utils.h"

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>

#include <CoreServices/CoreServices.h>

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + StringA("\n"), LM_LOG );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + StringA("\n"), LM_ERROR );
	
#define PATH_DELIM '/'
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
	static StringW s_UniCharFromCString( const String& _cstring )
	{
		TextEncoding encoding = CreateTextEncoding( CFStringGetSystemEncoding(), kTextEncodingDefaultVariant, kTextEncodingDefaultFormat );
		TextToUnicodeInfo info;
		OSStatus status = CreateTextToUnicodeInfoByEncoding( encoding, &info );
		UniChar conv[MAXPATHLEN];
		ByteCount a, b;
		status = ConvertFromTextToUnicode( info, _cstring.size(), _cstring.c_str(), 0, 0, 0, NULL, NULL, MAXPATHLEN, &a, &b, conv );
    		return StringW( (const wchar_t*)conv, b/sizeof(UniChar) );
	}
	//////////////////////////////////////////////////////////////////////////
	static String s_CStringFromUniChar( const StringW& _uniChar )
	{
		TextEncoding encoding = CreateTextEncoding( CFStringGetSystemEncoding(), kTextEncodingDefaultVariant, kTextEncodingDefaultFormat );
		UnicodeToTextInfo info;
		OSStatus status = CreateUnicodeToTextInfoByEncoding( encoding, &info );
		char conv[MAXPATHLEN];
		ByteCount a, b;
		status = ConvertFromUnicodeToText( info, _uniChar.size(), (const UniChar*)_uniChar.c_str(), 0, 0, 0, NULL, NULL, MAXPATHLEN, &a, &b, conv );
		return String( conv, b );
	}
	//////////////////////////////////////////////////////////////////////////
	static FSRef s_findParentDirRef( const String& _path, String& _rest )
	{
		FSRef ref;
		Boolean isDir = true;
		TStringVector paths = Utils::split( _path, "/" );
		if( paths.empty() != true )
		{
			paths[0] = PATH_DELIM + paths[0];
			for( TStringVector::size_type i = 1; i < paths.size(); i++ )
			{
				paths[i] = paths[i-1] + PATH_DELIM + paths[i];
			}
		}
		for( TStringVector::size_type i = paths.size() - 1; i >= 0; i-- )
		{
			OSStatus status = FSPathMakeRef( (const UInt8*)paths[i].c_str(), &ref, &isDir );
			if( status == 0 )
			{
				_rest = _path.substr( paths[i].size()+1, _path.size() );
				return ref;
			}
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
		DataStreamInterface* fileData = 0;

		LOG( "Opening file: \"" + _filename + "\"" );
		
		if( existFile( _filename ) == false )
		{
			LOG_ERROR( "file \"" + _filename + "\" does not exist" );
			return 0;
		}

		try
		{
			String full_path = s_concatenatePath( m_initPath, _filename );
			//StringW full_path_w = Utils::AToW( full_path );
			// Use filesystem to determine size 
			// (quicker than streaming to the end and back)
			struct stat tagStat;

			int ret = stat( full_path.c_str(), &tagStat );

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

			fileData = static_cast<DataStreamInterface*>(stream);

			/*if( !fileData )
			{
				LOG_ERROR( "unrecognized error while opening file \"" + _filename + "\"" );
				return fileData;
			}*/
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
		_stream->release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::existFile( const String& _filename )
	{
		String full_path = s_concatenatePath( m_initPath, _filename );
		//StringW full_path_w = Utils::AToW( full_path );

		struct stat tagStat;
		bool ret = ( stat( full_path.c_str(), &tagStat ) == 0 );
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::createFolder( const String& _path )
	{
		//String  fullPath = m_appDataPath + PATH_DELIM + _path;
		String subdir;
		FSRef pathRef = s_findParentDirRef( _path, subdir );
		
		TStringVector subdirs = Utils::split( subdir, "/" );
		for( TStringVector::size_type i = 0; i < subdirs.size(); i++ )
		{
			StringW _dirNameW = s_UniCharFromCString( subdirs[i] );
			FSRef newDir;
			OSErr err = FSCreateDirectoryUnicode( &pathRef, _dirNameW.size(), (const UniChar*)_dirNameW.c_str(), kFSCatInfoNone, NULL, &newDir, NULL, NULL );
			if( err == dupFNErr )	// already exist
			{
				// get FSRef
				FSMakeFSRefUnicode( &pathRef, _dirNameW.size(), (const UniChar*)_dirNameW.c_str(), kTextEncodingUnknown, &newDir );
			}
			else if( err != 0 )
			{
				return false;
			}
			pathRef = newDir;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::deleteFolder( const String& _path )
	{/*
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
			
			return false;
		}*/
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::initAppDataPath( const String& _path )
	{
		FSRef  folderRef;
		UInt8 path[MAXPATHLEN];
		OSErr err = FSFindFolder( kUserDomain, kApplicationSupportFolderType, kDontCreateFolder, &folderRef );
		if( err != 0 )
		{
			return false;
		}
		TStringVector subdirs = Utils::split( _path, "/" );

		FSRefMakePath( &folderRef, path, MAXPATHLEN );
		
		for( TStringVector::size_type i = 0; i < subdirs.size(); i++ )
		{
			FSRef newDir;
			StringW _dirNameW = s_UniCharFromCString( subdirs[i] );
			err = FSCreateDirectoryUnicode( &folderRef, _dirNameW.size(), (const UniChar*)_dirNameW.c_str(), kFSCatInfoNone, NULL, &newDir, NULL, NULL );
			if( err == dupFNErr )	// already exist
			{
				// get FSRef
				FSMakeFSRefUnicode( &folderRef, _dirNameW.size(), (const UniChar*)_dirNameW.c_str(), kTextEncodingUnknown, &newDir );
			}
			else if( err != 0 )
			{
				return false;
			}
			folderRef = newDir;
		}
		m_appDataPath = String( (const char*)path ) + PATH_DELIM + _path;
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
			filename = m_appDataPath + PATH_DELIM + _filename;
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
		/*SHFILEOPSTRUCT fs;
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
		}*/
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
