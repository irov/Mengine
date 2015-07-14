#	include "ResourceSpine.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"

#	include "Core/MemoryCacheBuffer.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceSpine );
	//////////////////////////////////////////////////////////////////////////
	ResourceSpine::ResourceSpine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceSpine::~ResourceSpine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSpine::_loader( const Metabuf::Metadata * _meta )
	{
		//const Metacode::Meta_DataBlock::Meta_ResourceCal3dSkeleton * metadata
		//	= static_cast<const Metacode::Meta_DataBlock::Meta_ResourceCal3dSkeleton *>(_meta);

		//m_filePath = metadata->get_File_Path();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSpine::_compile()
	{
		const ConstString & category = this->getCategory();
		
		MemoryCacheBufferPtr buffer = Helper::createMemoryFile( m_serviceProvider, category, m_filePath, false, "ResourceSpine::_compile" );

		if( buffer == nullptr )
		{
			return false;
		}
		
		void * memory_buffer = buffer->getMemory();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSpine::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSpine::_isValid() const
	{
		return true;
	}
}