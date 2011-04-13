#	include "ResourceEmitterContainer.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"

#	include "ResourceManager.h"
#	include "ResourceImageDefault.h"
#	include "ParticleEngine.h"
#	include "Logger/Logger.h"

#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceEmitterContainer );
	//////////////////////////////////////////////////////////////////////////
	ResourceEmitterContainer::ResourceEmitterContainer()
		: m_container(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setFilePath( const ConstString& _path )
	{
		m_filename = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setFolderPath( const ConstString& _path )
	{
		m_folder = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceEmitterContainer::getFilePath() const
	{
		return m_filename;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceEmitterContainer::getFolderPath() const
	{
		return m_folder;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::File_Path, &ResourceEmitterContainer::setFilePath );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Folder_Path, &ResourceEmitterContainer::setFolderPath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer::_compile()
	{
		const ConstString & category = this->getCategory();

		m_container = ParticleEngine::get()
			->createEmitterContainerFromFile( category, m_filename );

		if( m_container == 0 )
		{
			MENGE_LOG_ERROR( "Image can't create container file '%s'"
				, m_filename.c_str() 
				);

			return false;
		}

		const EmitterContainerInterface::TVectorAtlas & atlas = m_container->getAtlas();

		for( EmitterContainerInterface::TVectorAtlas::const_iterator
			it = atlas.begin(),
			it_end = atlas.end();
		it != it_end;
		++it )
		{
			ConstString fullname( Helper::to_str(m_folder) + "/" + it->file );

			if( ResourceManager::get()
				->hasResource( fullname ) == false )
			{
				this->createResource_( fullname );
			}
			
			ResourceImageDefault * image = ResourceManager::get()
				->getResourceT<ResourceImageDefault>( fullname );

			m_atlasImages.push_back( image );
		}

		return true;
	}
	void ResourceEmitterContainer::createResource_( const ConstString & _fullname )
	{
		const ConstString & category = this->getCategory();
		const ConstString & group = this->getGroup();

		ResourceImageDefault * image = ResourceManager::get()
			->createResourceT<ResourceImageDefault>( category, group, _fullname, Consts::get()->c_ResourceImageDefault );

		image->addImagePath( _fullname, mt::vec2f(-1.f,-1.f) );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::_release()
	{
		for( TVectorAtlasImages::iterator
			it = m_atlasImages.begin(),
			it_end = m_atlasImages.end();
		it != it_end;
		++it)
		{
			ResourceManager::get()
				->releaseResource( *it );
		}

		m_atlasImages.clear();

		if( m_container != 0 )
		{
			ParticleEngine::get()
				->releaseEmitterContainer( m_container );

			m_container = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault* ResourceEmitterContainer::getAtlasImage( std::size_t _atlasId )
	{
		return m_atlasImages[_atlasId];
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterContainerInterface * ResourceEmitterContainer::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
}
