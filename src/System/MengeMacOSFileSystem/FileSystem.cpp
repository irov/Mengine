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
	static String s_makeCorrectDelims( const String& _path )
	{
		String correct = _path;
		String::size_type pos = correct.find( "\\" );
		while( pos != String::npos )
		{
			correct[pos] = PATH_DELIM;
			pos = correct.find( "\\" ); 
		}
		return correct;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_isAbsolutePath( const String& _path )
	{
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
	/*static String s_UniCharFromCString( const String& _cstring )
	{
		TextEncoding encoding = CreateTextEncoding( CFStringGetSystemEncoding(), kTextEncodingDefaultVariant, kTextEncodingDefaultFormat );
		TextToUnicodeInfo info;
		OSStatus status = CreateTextToUnicodeInfoByEncoding( encoding, &info );
		UniChar conv[MAXPATHLEN];
		ByteCount a, b;
		status = ConvertFromTextToUnicode( info, _cstring.size(), _cstring.c_str(), 0, 0, 0, NULL, NULL, MAXPATHLEN, &a, &b, conv );
		DisposeTextToUnicodeInfo( &info );
		return String( (const char*)conv, b );
	}
	//////////////////////////////////////////////////////////////////////////
	static String s_CStringFromUniChar( const String& _uniChar )
	{
		TextEncoding encoding = CreateTextEncoding( kTextEncodingUnicodeDefault, kTextEncodingDefaultVariant, kUnicodeUTF8Format );
		TextToUnicodeInfo info;
		OSStatus status = CreateTextToUnicodeInfoByEncoding( encoding, &info );
		UniChar conv[MAXPATHLEN];
		ByteCount a, b;
		status = ConvertFromTextToUnicode( info, _uniChar.size(), _uniChar.c_str(), 0, 0, 0, NULL, NULL, MAXPATHLEN, &a, &b, conv );
		DisposeTextToUnicodeInfo( &info );
		return String( (const char*)conv, b );
	}*/
	static String s_utf8ToAnsi( const String& _utf8 )
	{
		OSStatus err = noErr;
		TECObjectRef tec = 0;
	    ByteCount bytesConsumed = 0;
		ByteCount bytesProduced = 0;

	    TextEncoding outputEncoding	= CreateTextEncoding( CFStringGetSystemEncoding(),
                                        kTextEncodingDefaultVariant,
                                        kTextEncodingDefaultFormat);

		TextEncoding inputEncoding = CreateTextEncoding( kTextEncodingUnicodeDefault,
                                        kTextEncodingDefaultVariant,
                                        kUnicodeUTF8Format);

		err = TECCreateConverter( &tec, inputEncoding, outputEncoding );

		std::size_t bufLen = _utf8.length();
		String out;
		char* buffer = new char[bufLen];
		if (err == noErr)
		{
			err = TECConvertText(tec,
                    (ConstTextPtr) _utf8.c_str(),
                    _utf8.length(),				// inputBufferLength
                    &bytesConsumed,				// actualInputLength
                    (TextPtr) buffer,			// outputBuffer
                    bufLen,					// outputBufferLength
                    &bytesProduced);			// actualOutputLength

			out.assign( buffer, bytesProduced );
			
			TECFlushText( tec, (TextPtr) buffer, bufLen, &bytesProduced );
			if( bytesProduced > 0 )
			{
				out += String( buffer, bytesProduced );
			}
			TECDisposeConverter(tec);
		}
		
		delete[] buffer;
		return out;
	}
	static String s_utf8ToUniChar( const String& _utf8 )
	{
		String ansi = s_utf8ToAnsi( _utf8 );
		TextEncoding encoding = CreateTextEncoding( CFStringGetSystemEncoding(), kTextEncodingDefaultVariant, kTextEncodingDefaultFormat );
		TextToUnicodeInfo info;
		OSStatus status = CreateTextToUnicodeInfoByEncoding( encoding, &info );
		UniChar conv[MAXPATHLEN];
		ByteCount a, b;
		status = ConvertFromTextToUnicode( info, ansi.size(), ansi.c_str(), 0, 0, 0, NULL, NULL, MAXPATHLEN, &a, &b, conv );
		DisposeTextToUnicodeInfo( &info );
		return String( (const char*)conv, b );
	}
	//////////////////////////////////////////////////////////////////////////
	FSRef FileSystem::s_findParentDirRef( const String& _path, String& _rest )
	{
		String fullPath = _path;
		FSRef ref;
		Boolean isDir = true;
		TStringVector paths = Utils::split( fullPath, "/" );
		// get root
		FSPathMakeRef( (const UInt8*)"/", &ref, &isDir );
		String full;
		for( TStringVector::size_type i = 0; i < paths.size(); i++ )
		{
			FSRef newRef;
			//String name = s_UniCharFromCString( paths[i] );
			String name = s_utf8ToUniChar( paths[i] );
			OSErr err = FSMakeFSRefUnicode( &ref, name.size()/sizeof(UniChar), (const UniChar*)name.c_str(), kTextEncodingUnknown, &newRef );
			if( err == 0 )	// ok
			{
				full += PATH_DELIM + paths[i];
				ref = newRef;
			}
			else // probably does not exist yet
			{
				_rest = _path.substr( full.length() + 1, _path.length() );
				break;
			}
		}
		return ref;
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
		m_initPath = s_makeCorrectDelims( _path );
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
		String filename_correct = s_makeCorrectDelims( _filename );

		LOG( "Opening file: \"" + filename_correct + "\"" );
		
		if( existFile( _filename ) == false )
		{
			LOG_ERROR( "file \"" + filename_correct + "\" does not exist" );
			return 0;
		}

		try
		{
			String full_path = s_concatenatePath( m_initPath, filename_correct );
			//String full_path_utf8 = s_ANSIToUTF8( full_path );
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
				new FileStreamDataStream( origStream, tagStat.st_size );

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
	DataStreamInterface* FileSystem::createMemoryFile( void* _data, std::streamsize _size )
	{
		return static_cast<DataStreamInterface*>( new MemoryDataStream( _data, _size ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystem::closeStream( DataStreamInterface* _stream )
	{
		_stream->release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::existFile( const String& _filename )
	{
		String filename_correct = s_makeCorrectDelims( _filename );
		String full_path = s_concatenatePath( m_initPath, filename_correct );

		struct stat tagStat;
		bool ret = ( stat( full_path.c_str(), &tagStat ) == 0 );
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::createFolder( const String& _path )
	{
		String path_correct = s_makeCorrectDelims( _path );
		String subdir;
		FSRef pathRef = FileSystem::s_findParentDirRef( path_correct, subdir );
		
		if( subdir.empty() == true )
		{
			return true;
		}
		TStringVector subdirs = Utils::split( subdir, "/" );
		for( TStringVector::size_type i = 0; i < subdirs.size(); i++ )
		{
			String _dirName = s_utf8ToUniChar( subdirs[i] );
			FSRef newDir;
			OSErr err = FSCreateDirectoryUnicode( &pathRef, _dirName.size()/sizeof(UniChar), (const UniChar*)_dirName.c_str(), kFSCatInfoNone, NULL, &newDir, NULL, NULL );
			if( err == dupFNErr )	// already exist
			{
				// get FSRef
				FSMakeFSRefUnicode( &pathRef, _dirName.size()/sizeof(UniChar), (const UniChar*)_dirName.c_str(), kTextEncodingUnknown, &newDir );
			}
			else if( err != 0 )
			{
				return false;
			}
			pathRef = newDir;
		}

		return true;
	}
	static OSStatus s_deleteFolder( const FSRef* folderRef )
	{
		FSIterator iterator;
		FSRef itemToDelete;
		UInt16 nodeFlags;
		ItemCount actualObjects = 0;
		FSCatalogInfo catInfo;
		
		OSStatus err = FSOpenIterator( folderRef, kFSIterateFlat | kFSIterateDelete, &iterator );
		if( err != noErr )
		{
			return err;
		}
		do 	// delete the contents of the directory
		{
			// get 1 item to delete
			err = FSGetCatalogInfoBulk(	iterator, 1, &actualObjects, NULL, kFSCatInfoNodeFlags, &catInfo, &itemToDelete, NULL, NULL);
			if ( (err == noErr) && (actualObjects == 1) )
			{
				// save node flags in local in case we have to recurse */
				nodeFlags = catInfo.nodeFlags;

				// is it a directory?
				if ( (nodeFlags & kFSNodeIsDirectoryMask) != 0 )
				{
					err = s_deleteFolder( &itemToDelete );
				}
				if ( err == noErr)			// are we still OK to delete?
				{
					if ( (nodeFlags & kFSNodeLockedMask) != 0 )	// is item locked?
					{		// then attempt to unlock it (ignore result since FSDeleteObject will set it correctly)
						catInfo.nodeFlags = nodeFlags & ~kFSNodeLockedMask;
						(void) FSSetCatalogInfo(&itemToDelete, kFSCatInfoNodeFlags, &catInfo);
					}
					// delete the item
					err = FSDeleteObject(&itemToDelete);
				}
			}
		} 
		while ( err == noErr );
		
		FSCloseIterator(iterator);
		err = FSDeleteObject(folderRef);
		return err;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::deleteFolder( const String& _path )
	{
		String path_correct = s_makeCorrectDelims( _path );
		String path = s_concatenatePath( m_initPath, path_correct );

		String subdir;
		FSRef folderRef = s_findParentDirRef( path, subdir );
		if( subdir.empty() == false ) // something wrong
		{
			return false;
		}
		return ( s_deleteFolder( &folderRef ) == noErr );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystem::initAppDataPath( const String& _path, bool _local )
	{
		if( _local == true )
		{
			m_appDataPath = _path;
			return true;
		}
		String path_correct = s_makeCorrectDelims( _path );
		FSRef  folderRef;
		UInt8 path[MAXPATHLEN];
		OSErr err = FSFindFolder( kUserDomain, kApplicationSupportFolderType, kDontCreateFolder, &folderRef );
		if( err != 0 )
		{
			return false;
		}
		TStringVector subdirs = Utils::split( path_correct, "/" );

		FSRefMakePath( &folderRef, path, MAXPATHLEN );
		
		for( TStringVector::size_type i = 0; i < subdirs.size(); i++ )
		{
			FSRef newDir;
			String _dirName = s_utf8ToUniChar( subdirs[i] );
			err = FSCreateDirectoryUnicode( &folderRef, _dirName.size()/sizeof(UniChar), (const UniChar*)_dirName.c_str(), kFSCatInfoNone, NULL, &newDir, NULL, NULL );
			if( err == dupFNErr )	// already exist
			{
				// get FSRef
				FSMakeFSRefUnicode( &folderRef, _dirName.size()/sizeof(UniChar), (const UniChar*)_dirName.c_str(), kTextEncodingUnknown, &newDir );
			}
			else if( err != 0 )
			{
				return false;
			}
			folderRef = newDir;
		}
		m_appDataPath = String( (const char*)path ) + PATH_DELIM + path_correct;
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
		String filename_correct = s_makeCorrectDelims( _filename );
		String filename;
		if( m_appDataPath.empty() )
		{
			filename = filename_correct;
		}
		else
		{
			filename = m_appDataPath + PATH_DELIM + filename_correct;
		}

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
		String filename_correct = s_makeCorrectDelims( _filename );
		String subdir;
		String filename = s_concatenatePath( m_initPath, filename_correct );
		FSRef ref = s_findParentDirRef( filename, subdir );
		if( subdir.empty() == false ) // can't find FSRef - something wrong
		{
			return false;
		}
		OSStatus err = FSDeleteObject( &ref );
		if( err != noErr )
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
	bool FileSystem::isAbsolutePath( const String& _path )
	{
		return s_isAbsolutePath( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	String FileSystem::joinPath( const String& _base, const String& _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
}
