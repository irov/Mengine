#	include "ResourceCal3dAnimation.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"

#	include "Core/CacheMemoryBuffer.h"

#	include "cal3d/loader.h"
#	include "cal3d/buffersource.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceCal3dAnimation);
	//////////////////////////////////////////////////////////////////////////
	ResourceCal3dAnimation::ResourceCal3dAnimation()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceCal3dAnimation::~ResourceCal3dAnimation()
	{
	}
	const CalCoreAnimationPtr & ResourceCal3dAnimation::getAnimation() const
	{
		return m_animation;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCal3dAnimation::_loader( const Metabuf::Metadata * _meta )
	{
		const Metacode::Meta_DataBlock::Meta_ResourceCal3dAnimation * metadata
			= static_cast<const Metacode::Meta_DataBlock::Meta_ResourceCal3dAnimation *>(_meta);

		m_filePath = metadata->get_File_Path();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCal3dAnimation::_compile()
	{
		const ConstString & category = this->getCategory();

		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_filePath, false );

		size_t size = stream->size();
		CacheMemoryBuffer buffer(m_serviceProvider, size, "ResourceCal3dAnimation::_compile");
		void * memory_buffer = buffer.getMemory();

		stream->read( memory_buffer, size );

		CalBufferSource bufferSrc(memory_buffer);

		m_animation = CalLoader::loadCoreAnimation( bufferSrc );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceCal3dAnimation::_release()
	{
		m_animation = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCal3dAnimation::_isValid() const
	{
		return true;
	}
}