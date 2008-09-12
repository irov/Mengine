
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
	FileManager::FileManager( const Text& _initPath )
		: m_initPath( _initPath )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void FileManager::setInitPath( const Text& _path )
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
	static bool s_isReservedDir( const Text& _fn )
	{
		return ( _fn [0] == MENGE_TEXT('.') && ( _fn [1] == 0 || ( _fn [1] == MENGE_TEXT('.') && _fn [2] == 0 ) ) );
	}
	//////////////////////////////////////////////////////////////////////////
	FileManager::~FileManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DataStream* FileManager::open( const Text& _filename ) const
	{

	}
}