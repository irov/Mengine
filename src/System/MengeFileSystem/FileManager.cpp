
#	include "FileManager.h"
#	include "FileData.h"

#	include "Config/Config.h"

#	include <sys/types.h>
#	include <sys/stat.h>
#	include <assert.h>
#	include <fstream>

#if TARGET_PLATFORM == TP_LINUX || TARGET_PLATFORM == TP_APPLE
//#   include "OgreSearchOps.h"
#   include <sys/param.h>
#   define MAX_PATH MAXPATHLEN
#endif

#if TARGET_PLATFORM == TP_WINDOWS
#   include <windows.h>
#   include <direct.h>
#   include <io.h>
#endif

//////////////////////////////////////////////////////////////////////////
FileManager::FileManager( const std::string& _initPath )
: m_initPath( _initPath )
{
}
//////////////////////////////////////////////////////////////////////////
void FileManager::setInitPath( const std::string& _path )
{
	m_initPath = _path;
}
//////////////////////////////////////////////////////////////////////////
bool FileManager::isCaseSensitive() const
{
#if TARGET_PLATFORM == TP_WINDOWS
	return false;
#else
	return true;
#endif
}
//////////////////////////////////////////////////////////////////////////
static bool s_isReservedDir( const char *_fn )
{
	return ( _fn [0] == '.' && ( _fn [1] == 0 || ( _fn [1] == '.' && _fn [2] == 0 ) ) );
}
//////////////////////////////////////////////////////////////////////////
static bool s_isAbsolutePath( const char* _path )
{
#if TARGET_PLATFORM == TP_WINDOWS
	if ( ::isalpha( unsigned char( _path[0] ) ) && _path[1] == ':' )
	{
		return true;
	}
#endif
	return _path[0] == '/' || _path[0] == '\\';
}
//////////////////////////////////////////////////////////////////////////
static std::string s_concatenatePath( const std::string& _base, const std::string& _name )
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
void FileManager::findFiles( const std::string& _pattern, bool _recursive, 
	bool _dirs, TStringVector* _simpleList, TFileInfoVector* _detailList )
{
	long lHandle, res;
	struct _finddata_t tagData;

	// pattern can contain a directory name, separate it from mask
	std::size_t pos1 = _pattern.rfind ('/');
	std::size_t pos2 = _pattern.rfind ('\\');
	if ( pos1 == _pattern.npos || ( ( pos2 != _pattern.npos ) && ( pos1 < pos2 ) ) )
	{
		pos1 = pos2;
	}
	std::string directory;
	if ( pos1 != _pattern.npos )
	{
		directory = _pattern.substr (0, pos1 + 1);
	}

	std::string full_pattern = s_concatenatePath( m_initPath, _pattern );

	lHandle = static_cast<long>( _findfirst( full_pattern.c_str(), &tagData ) );
	res = 0;
	while ( lHandle != -1 && res != -1 )
	{
		if ( ( _dirs == ( ( tagData.attrib & _A_SUBDIR ) != 0 ) ) &&
			( !_dirs || !s_isReservedDir( tagData.name ) ) )
		{
			if( _simpleList )
			{
				_simpleList->push_back( directory + tagData.name );
			}
			else if( _detailList )
			{
				FileInfo fi;
				fi.filename = directory + tagData.name;
				fi.basename = tagData.name;
				fi.path = directory;
				fi.compressedSize = tagData.size;
				fi.uncompressedSize = tagData.size;
				_detailList->push_back(fi);
			}
		}
		res = _findnext( lHandle, &tagData );
	}
	// Close if we found any files
	if( lHandle != -1 )
	{
		_findclose( lHandle );
	}

	// Now find directories
	if( _recursive )
	{
		std::string base_dir = m_initPath;
		if ( !directory.empty() )
		{
			base_dir = s_concatenatePath( m_initPath, directory );
			// Remove the last '/'
			base_dir.erase( base_dir.length() - 1);
		}
		base_dir.append("/*");

		// Remove directory name from pattern
		std::string mask("/");
		if( pos1 != _pattern.npos )
		{
			mask.append( _pattern.substr( pos1 + 1 ) );
		}
		else
		{
			mask.append( _pattern );
		}

		lHandle = static_cast<long>( _findfirst( base_dir.c_str(), &tagData ) );
		res = 0;
		while (lHandle != -1 && res != -1)
		{
			if ((tagData.attrib & _A_SUBDIR) &&
				!s_isReservedDir( tagData.name ) )
			{
				// recurse
				base_dir = directory;
				base_dir.append( tagData.name ).append( mask );
				findFiles( base_dir, _recursive, _dirs, _simpleList, _detailList );
			}
			res = _findnext( lHandle, &tagData );
		}
		// Close if we found any files
		if( lHandle != -1 )
		{
			_findclose( lHandle );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
FileManager::~FileManager()
{
}
//////////////////////////////////////////////////////////////////////////
DataStream* FileManager::open( const std::string& _filename ) const
{
	std::string full_path = s_concatenatePath( m_initPath, _filename );

	// Use filesystem to determine size 
	// (quicker than streaming to the end and back)
	struct stat tagStat;
	int ret = stat( full_path.c_str(), &tagStat );
	assert( ret == 0 && "Problem getting file size" );

	// Always open in binary mode
	std::ifstream *origStream = new std::ifstream();
	origStream->open( full_path.c_str(), std::ios::in | std::ios::binary );

	// Should check ensure open succeeded, in case fail for some reason.
	if ( origStream->fail() )
	{
		delete origStream;
		/*OGRE_EXCEPT(Exception::ERR_FILE_NOT_FOUND,
			"Cannot open file: " + filename,
			"FileSystemArchive::open");*/
	}

	/// Construct return stream, tell it to delete on destroy
	FileStreamDataStream* stream = new FileStreamDataStream( _filename,
		origStream, tagStat.st_size, true );
	return static_cast<DataStream*>(stream);
}
//////////////////////////////////////////////////////////////////////////
TStringVector* FileManager::list( bool _recursive, bool _dirs )
{
	// directory change requires locking due to saved returns
	TStringVector* ret( new TStringVector() );

	findFiles("*", _recursive, _dirs, ret, 0);

	return ret;
}
//////////////////////////////////////////////////////////////////////////
TFileInfoVector* FileManager::listFileInfo( bool _recursive, bool _dirs )
{
	TFileInfoVector* ret( new TFileInfoVector() );

	findFiles( "*", _recursive, _dirs, 0, ret );

	return ret;
}
//////////////////////////////////////////////////////////////////////////
TStringVector* FileManager::find( const std::string& _pattern, bool _recursive, bool _dirs )
{
	TStringVector* ret( new TStringVector() );

	findFiles( _pattern, _recursive, _dirs, ret, 0 );

	return ret;
}
//////////////////////////////////////////////////////////////////////////
TFileInfoVector* FileManager::findFileInfo( const std::string& _pattern, 
	bool _recursive, bool _dirs )
{
	TFileInfoVector* ret( new TFileInfoVector() );

	findFiles( _pattern, _recursive, _dirs, 0, ret );

	return ret;
}
//////////////////////////////////////////////////////////////////////////
bool FileManager::exists( const std::string& _filename )
{
	std::string full_path = s_concatenatePath( m_initPath, _filename );

	struct stat tagStat;
	bool ret = ( stat( full_path.c_str(), &tagStat ) == 0 );

	return ret;
}
//////////////////////////////////////////////////////////////////////////