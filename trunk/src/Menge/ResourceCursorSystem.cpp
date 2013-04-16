#	include "ResourceCursorSystem.h"
#	include "Kernel/ResourceImplement.h"

#	include "Metacode.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceCursorSystem );
	//////////////////////////////////////////////////////////////////////////
	ResourceCursorSystem::ResourceCursorSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCursorSystem::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceCursorSystem * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceCursorSystem *>(_meta);

        metadata->swap_File_Path( m_path );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceCursorSystem::getPath() const
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