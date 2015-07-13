#	include "ResourceSpine.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"

#	include "Core/CacheMemoryBuffer.h"

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

		InputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
			->openInputFile( category, m_filePath, false );

		size_t size = stream->size();
		CacheMemoryBuffer buffer( m_serviceProvider, size, "ResourceCal3dAnimation::_compile" );
		void * memory_buffer = buffer.getMemory();

		stream->read( memory_buffer, size );

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