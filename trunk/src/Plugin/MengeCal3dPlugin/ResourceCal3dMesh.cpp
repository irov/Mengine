#	include "ResourceCal3dMesh.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"

#	include "Core/CacheMemoryBuffer.h"

#	include "cal3d/loader.h"
#	include "cal3d/buffersource.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceCal3dMesh);
	//////////////////////////////////////////////////////////////////////////
	ResourceCal3dMesh::ResourceCal3dMesh()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceCal3dMesh::~ResourceCal3dMesh()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const CalCoreMeshPtr & ResourceCal3dMesh::getMesh() const
	{
		return m_mesh;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCal3dMesh::_loader( const Metabuf::Metadata * _meta )
	{
		const Metacode::Meta_DataBlock::Meta_ResourceCal3dMesh * metadata
			= static_cast<const Metacode::Meta_DataBlock::Meta_ResourceCal3dMesh *>(_meta);

		m_filePath = metadata->get_File_Path();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCal3dMesh::_compile()
	{
		const ConstString & category = this->getCategory();

		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_filePath, false );
		
		size_t size = stream->size();
		CacheMemoryBuffer buffer(m_serviceProvider, size, "ResourceCal3dMesh::_compile");
		void * memory_buffer = buffer.getMemory();

		stream->read( memory_buffer, size );

		CalBufferSource bufferSrc(memory_buffer);

		m_mesh = CalLoader::loadCoreMesh( bufferSrc );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceCal3dMesh::_release()
	{
		m_mesh = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCal3dMesh::_isValid() const
	{
		return true;
	}
}