#	include "ResourceCursorSystem.h"
#	include "ResourceImplement.h"

#	include "Metacode.h"
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
	void ResourceCursorSystem::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceCursorSystem * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceCursorSystem *>(_meta);

        metadata->swap_File_Path( m_path );
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