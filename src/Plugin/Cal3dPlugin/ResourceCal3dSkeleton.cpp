#	include "ResourceCal3dSkeleton.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"

#	include "Core/CacheMemoryBuffer.h"

#	include "cal3d/loader.h"
#	include "cal3d/buffersource.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceCal3dSkeleton);
	//////////////////////////////////////////////////////////////////////////
	ResourceCal3dSkeleton::ResourceCal3dSkeleton()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceCal3dSkeleton::~ResourceCal3dSkeleton()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const CalCoreSkeletonPtr & ResourceCal3dSkeleton::getSkeleton() const
	{
		return m_skeleton;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCal3dSkeleton::_loader( const Metabuf::Metadata * _meta )
	{
		const Metacode::Meta_DataBlock::Meta_ResourceCal3dSkeleton * metadata
			= static_cast<const Metacode::Meta_DataBlock::Meta_ResourceCal3dSkeleton *>(_meta);

		m_filePath = metadata->get_File_Path();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCal3dSkeleton::_compile()
	{
		const ConstString & category = this->getCategory();

		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_filePath, false );

		size_t size = stream->size();
		CacheMemoryBuffer buffer(m_serviceProvider, size, "ResourceCal3dSkeleton::_compile");
		void * memory_buffer = buffer.getMemory();

		stream->read( memory_buffer, size );

		CalBufferSource bufferSrc(memory_buffer);

		m_skeleton = CalLoader::loadCoreSkeleton( bufferSrc );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceCal3dSkeleton::_release()
	{
		m_skeleton = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCal3dSkeleton::_isValid() const
	{
		return true;
	}
}