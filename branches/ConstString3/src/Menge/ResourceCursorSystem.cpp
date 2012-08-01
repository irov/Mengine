#	include "ResourceCursorSystem.h"
#	include "ResourceImplement.h"

#	include "BinParser.h"
#	include "LoaderEngine.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceCursorSystem );
	//////////////////////////////////////////////////////////////////////////
	ResourceCursorSystem::ResourceCursorSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceCursorSystem::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::File_Path, m_path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & ResourceCursorSystem::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	void * ResourceCursorSystem::getBuffer( size_t & _size ) const
	{
		_size = 0;

		return 0;
	}
}