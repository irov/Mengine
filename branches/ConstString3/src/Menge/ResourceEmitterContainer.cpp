#	include "ResourceEmitterContainer.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

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
	void ResourceEmitterContainer::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceEmitterContainer::setFilePath );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Folder", "Path", &ResourceEmitterContainer::setFolderPath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer::_compile()
	{
		const ConstString & category = this->getCategory();

		m_container = ParticleEngine::get()
			->createEmitterContainerFromFile( category, m_filename.str() );

		if( m_container == 0 )
		{
			MENGE_LOG_ERROR( "Image can't create container file '%s'"
				, m_filename.c_str() 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::_release()
	{
		for( TMapImageEmitters::iterator
			it = m_mapImageEmitters.begin(),
			it_end = m_mapImageEmitters.end();
		it != it_end;
		++it)
		{
			Holder<ResourceManager>::get()
				->releaseResource( it->second );
		}

		m_mapImageEmitters.clear();

		if( m_container != 0 )
		{
			Holder<ParticleEngine>::get()
				->releaseEmitterContainer( m_container );
			m_container = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault* ResourceEmitterContainer::getRenderImage( const char * _name )
	{
		TMapImageEmitters::iterator it = m_mapImageEmitters.find( _name );

		if ( it == m_mapImageEmitters.end() )
		{
			const ConstString & category = this->getCategory();
			const ConstString & group = this->getGroup();

			ConstString fullname = m_folder.str() + _name;

			ResourceImageDefault* image = ResourceManager::get()
				->getResourceT<ResourceImageDefault>( fullname );

			if( image == 0 )
			{
				ResourceFactoryParam params 
					= { fullname, category, group };

				image = ResourceManager::get()
					->createResourceWithParamT<ResourceImageDefault>( Consts::get()->c_ResourceImageDefault, params );

				//ConstString folder = ConstManager::get()
				//	->genString( m_folder.str() + _name );
				//image->addImagePath( folder );

				image->addImagePath( fullname );

				ResourceManager::get()
					->registerResource( image );
				
				image = ResourceManager::get()
					->getResourceT<ResourceImageDefault>( fullname );
			}

			m_mapImageEmitters.insert( std::make_pair( _name, image ) );

			return image;
		}

		return (*it).second;
	}
	//////////////////////////////////////////////////////////////////////////
	const EmitterContainerInterface * ResourceEmitterContainer::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
}
