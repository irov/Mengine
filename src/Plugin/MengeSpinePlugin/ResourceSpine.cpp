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
		: m_atlas(nullptr)
		, m_skeletonData(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceSpine::~ResourceSpine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSpine::_loader( const Metabuf::Metadata * _meta )
	{
		const Metacode::Meta_DataBlock::Meta_ResourceSpine * metadata
			= static_cast<const Metacode::Meta_DataBlock::Meta_ResourceSpine *>(_meta);

		m_skeletonPath = metadata->get_Skeleton_Path();
		m_atlasPath = metadata->get_Atlas_Path();
		m_texturePath = metadata->get_Texture_Path();
		m_textureCodecName = metadata->get_Texture_Codec();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderTextureInterfacePtr & ResourceSpine::getAtlasTexture() const
	{
		return m_texture;
	}
	//////////////////////////////////////////////////////////////////////////
	spAtlas * ResourceSpine::getAtlas()
	{
		return m_atlas;
	}
	//////////////////////////////////////////////////////////////////////////
	spSkeletonData * ResourceSpine::getSkeletonData()
	{
		return m_skeletonData;
	}
	//////////////////////////////////////////////////////////////////////////
	extern "C" void _spAtlasPage_createTexture( spAtlasPage * _page, const char * _path )
	{
		(void)_path;

		ResourceSpine * resource = (ResourceSpine*)_page->rendererObject;

		const RenderTextureInterfacePtr & texture = resource->getAtlasTexture();

		_page->width = texture->getWidth();
		_page->height = texture->getHeight();
	}
	//////////////////////////////////////////////////////////////////////////
	extern "C" void _spAtlasPage_disposeTexture( spAtlasPage * _page )
	{
		(void)_page;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	extern "C" char * _spUtil_readFile( const char* path, int* length )
	{
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSpine::_compile()
	{
		const ConstString & category = this->getCategory();

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE( m_serviceProvider )
			->loadTexture( category, m_texturePath, m_textureCodecName );

		if( texture == nullptr )
		{
			return false;
		}

		m_texture = texture;

		MemoryCacheBufferPtr atlas_buffer = Helper::createMemoryFileString( m_serviceProvider, category, m_atlasPath, false, "ResourceSpine::_compile m_atlasPath" );

		if( atlas_buffer == nullptr )
		{
			return false;
		}

		const char * atlas_memory = atlas_buffer->getMemoryT<const char *>();
		size_t atlas_size = atlas_buffer->getSize();

		spAtlas * atlas = spAtlas_create( atlas_memory, atlas_size, "", this );

		atlas_buffer = nullptr;

		spSkeletonJson * skeletonJson = spSkeletonJson_create( atlas );
		
		MemoryCacheBufferPtr skeleton_buffer = Helper::createMemoryFileString( m_serviceProvider, category, m_skeletonPath, false, "ResourceSpine::_compile m_skeletonPath" );

		if( skeleton_buffer == nullptr )
		{
			return false;
		}
		
		const char * json_memory = skeleton_buffer->getMemoryT<const char *>();
		
		spSkeletonData * skeletonData = spSkeletonJson_readSkeletonData( skeletonJson, json_memory );

		skeleton_buffer = nullptr;

		spSkeletonJson_dispose( skeletonJson );

		m_atlas = atlas;
		m_skeletonData = skeletonData;
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSpine::_release()
	{
		m_texture = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSpine::_isValid() const
	{
		return true;
	}
}