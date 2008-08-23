
#	include "FileManager.h"
#	include "FileStreamDataStream.h"

#	include "Config/Config.h"

#	include <sys/stat.h>
#	include <sys/types.h>
#	include <assert.h>
#	include <fstream>

#if MENGE_PLATFORM == MENGE_PLATFORM_LINUX || MENGE_PLATFORM == MENGE_PLATFORM_APPLE
//#   include "OgreSearchOps.h"
#   include <sys/param.h>
#   define MAX_PATH MAXPATHLEN
#endif

#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS
#   include <windows.h>
#   include <direct.h>
#   include <io.h>
#endif

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileManager::FileManager( const String& _initPath )
		: m_initPath( _initPath )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void FileManager::setInitPath( const String& _path )
	{
		m_initPath = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileManager::isCaseSensitive() const
	{
#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS
		return false;
#else
		return true;
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_isReservedDir( const String& _fn )
	{
		return ( _fn [0] == MENGE_TEXT('.') && ( _fn [1] == 0 || ( _fn [1] == MENGE_TEXT('.') && _fn [2] == 0 ) ) );
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_isAbsolutePath( const String& _path )
	{
#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS
		if ( ::isalpha( unsigned char( _path[0] ) ) && _path[1] == ':' )
		{
			return true;
		}
#endif
		return _path[0] == MENGE_TEXT('/') || _path[0] == MENGE_TEXT('\\');
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
			return _base + MENGE_TEXT('/') + _name;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	FileManager::~FileManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DataStream* FileManager::open( const String& _filename ) const
	{
		String full_path = s_concatenatePath( m_initPath, _filename );

		// Use filesystem to determine size 
		// (quicker than streaming to the end and back)
		struct _stat tagStat;
#ifdef MENGE_UNICODE
		int ret = _wstat( full_path.c_str(), &tagStat );
#else
		int ret = _stat( full_path.c_str(), &tagStat );
#endif
		assert( ret == 0 && "Problem getting file size" );

		std::ifstream *origStream = new std::ifstream();

#ifdef MENGE_UNICODE
		// Always open in binary mode
		origStream->open( full_path.c_str(), std::ios::in | std::ios::binary );
#else
		wchar_t lpszW[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, full_path.c_str(), -1, lpszW, full_path.size() );
		lpszW[full_path.size()] = 0;

		// Always open in binary mode
		origStream->open( lpszW, std::ios::in | std::ios::binary );
#endif

		// Should check ensure open succeeded, in case fail for some reason.
		if ( origStream->fail() )
		{
			delete origStream;
		}

		/// Construct return stream, tell it to delete on destroy
		FileStreamDataStream* stream = 
			new FileStreamDataStream( _filename, origStream, tagStat.st_size, true );

		return static_cast<DataStream*>(stream);
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileManager::exists( const String& _filename )
	{
		String full_path = s_concatenatePath( m_initPath, _filename );

		struct _stat tagStat;
#ifdef MENGE_UNICODE
		bool ret = ( _wstat( full_path.c_str(), &tagStat ) == 0 );
#else
		bool ret = ( _stat( full_path.c_str(), &tagStat ) == 0 );
#endif

		return ret;
	}
}