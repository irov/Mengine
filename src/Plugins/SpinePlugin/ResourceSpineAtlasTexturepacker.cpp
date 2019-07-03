#	include "ResourceSpine.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/ResourceInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Metacode/Metacode.h"

#	include "Core/MemoryHelper.h"

#	include "Logger/Logger.h"

namespace Mengine
{
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

		const Metacode::Meta_DataBlock::Meta_ResourceSpine::TVectorMeta_Image & includes_images = metadata->get_IncludesImage();

		for( Metacode::Meta_DataBlock::Meta_ResourceSpine::TVectorMeta_Image::const_iterator
			it = includes_images.begin(),
			it_end = includes_images.end();
		it != it_end;
		++it )
		{
			const Metacode::Meta_DataBlock::Meta_ResourceSpine::Meta_Image & meta_image = *it;

			const ConstString & name = meta_image.get_Name();
			const ConstString & resourceName = meta_image.get_Resource();

			ResourceImagePtr image = RESOURCE_SERVICE( m_serviceProvider )
				->getResourceReferenceT<ResourceImagePtr>( resourceName );

			if( image == nullptr )
			{
				return false;
			}

			const mt::vec2f & ms = image->getMaxSize();
			
			if( mt::equal_f_f( ms.x, ms.y ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceSpine::_loader %s resource image %s not square texture %f:%f"
					, this->getName().c_str()
					, resourceName.c_str()
					, ms.x
					, ms.y
					);

				return false;
			}

			ImageDesc desc;
			desc.name = name;
			desc.image = image;

			m_images.push_back( desc );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImagePtr ResourceSpine::getResourceImage_( const char * _name ) const
	{
		for( TVectorImageDesc::const_iterator
			it = m_images.begin(),
			it_end = m_images.end();
		it != it_end;
		++it )
		{
			const ImageDesc & desc = *it;

			if( desc.name != _name )
			{
				continue;
			}

			return desc.image;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	spAtlas * ResourceSpine::getAtlas() const
	{
		return m_atlas;
	}
	//////////////////////////////////////////////////////////////////////////
	spSkeletonData * ResourceSpine::getSkeletonData() const
	{
		return m_skeletonData;
	}
	//////////////////////////////////////////////////////////////////////////
	spAnimation * ResourceSpine::findSkeletonAnimation( const ConstString & _name ) const
	{
		spAnimation * animation = spSkeletonData_findAnimation( m_skeletonData, _name.c_str() );

		return animation;
	}
	//////////////////////////////////////////////////////////////////////////
	extern "C" void _spAtlasPage_createTexture( spAtlasPage * _page, const char * _path )
	{
		ResourceSpine * resource = (ResourceSpine*)_page->atlas->rendererObject;

		ResourceImagePtr resourceImage = resource->getResourceImage_( _path );

		if( resourceImage == nullptr )
		{
			return;
		}

		const mt::vec2f & size = resourceImage->getMaxSize();

		_page->width = (int)size.x;
		_page->height = (int)size.y;
		_page->rendererObject = (void *)resourceImage.get();
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
		(void)path;
		(void)length;

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSpine::_compile()
	{
		for( TVectorImageDesc::iterator
			it = m_images.begin(),
			it_end = m_images.end();
		it != it_end;
		++it )
		{
			const ResourceImagePtr & image = it->image;

			if( image->incrementReference() == false )
			{
				return false;
			}
		}

		const ConstString & category = this->getCategory();

		MemoryCacheBufferInterfacePtr atlas_buffer = Helper::createMemoryCacheFile( m_serviceProvider, category, m_atlasPath, false, "ResourceSpine::_compile m_atlasPath" );

		if( atlas_buffer == nullptr )
		{
			return false;
		}

		const char * atlas_memory = atlas_buffer->getMemory();
		size_t atlas_size = atlas_buffer->getSize();

		int spine_atlas_size = (int)atlas_size;

		spAtlas * atlas = spAtlas_create( atlas_memory, spine_atlas_size, "", this );

		atlas_buffer = nullptr;

		spSkeletonJson * skeletonJson = spSkeletonJson_create( atlas );
		
		MemoryCacheBufferInterfacePtr skeleton_buffer = Helper::createMemoryCacheFileString( m_serviceProvider, category, m_skeletonPath, false, "ResourceSpine::_compile m_skeletonPath" );

		if( skeleton_buffer == nullptr )
		{
			return false;
		}
		
		const char * json_memory = skeleton_buffer->getMemory();
		
		spSkeletonData * skeletonData = spSkeletonJson_readSkeletonData( skeletonJson, json_memory );

		skeleton_buffer = nullptr;

		spSkeletonJson_dispose( skeletonJson );

		if( skeletonData == nullptr )
		{
			spAtlas_dispose( atlas );

			for( TVectorImageDesc::iterator
				it = m_images.begin(),
				it_end = m_images.end();
			it != it_end;
			++it )
			{
				const ResourceImagePtr & image = it->image;

				image->decrementReference();
			}

			return false;
		}

		m_atlas = atlas;
		m_skeletonData = skeletonData;
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceSpine::_release()
	{
		for( TVectorImageDesc::iterator
			it = m_images.begin(),
			it_end = m_images.end();
		it != it_end;
		++it )
		{
			const ResourceImagePtr & image = it->image;

			image->decrementReference();
		}

		if( m_skeletonData != nullptr )
		{
			spSkeletonData_dispose( m_skeletonData );
			m_skeletonData = nullptr;
		}

		if( m_atlas != nullptr )
		{
			spAtlas_dispose( m_atlas );
			m_atlas = nullptr;
		}				
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceSpine::_isValid() const
	{
		return true;
	}
}