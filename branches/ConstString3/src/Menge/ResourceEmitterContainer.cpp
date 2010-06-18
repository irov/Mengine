#	include "ResourceEmitterContainer.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "Factory/FactoryIdentity.h"

#	include "ResourceManager.h"
#	include "ResourceImageDefault.h"
#	include "ParticleEngine.h"
#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceEmitterContainer );
	//////////////////////////////////////////////////////////////////////////
	ResourceEmitterContainer::ResourceEmitterContainer()
		: m_container( 0 )
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

		m_container = ParticleEngine::hostage()
			->createEmitterContainerFromFile( category, m_filename );

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
			Holder<ResourceManager>::hostage()
				->releaseResource( it->second );
		}

		m_mapImageEmitters.clear();

		if( m_container != 0 )
		{
			Holder<ParticleEngine>::hostage()
				->releaseEmitterContainer( m_container );
			m_container = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault* ResourceEmitterContainer::getRenderImage( const ConstString& _name )
	{
		const ConstString & category = this->getCategory();

		String fullname = category.str() + "/" + m_folder.str() + _name;

		TMapImageEmitters::iterator it = m_mapImageEmitters.find( fullname );

		if ( it == m_mapImageEmitters.end() )
		{
			ResourceImageDefault* image = ResourceManager::hostage()
				->getResourceT<ResourceImageDefault>( fullname );

			const ConstString & category = this->getCategory();
			const ConstString & group = this->getGroup();
			const ConstString & file = this->getFile();

			if( image == 0 )
			{
				ResourceFactoryParam params 
					= { name, category, group, file };

				ConstString resourceType = ConstManager::hostage()
					->genString( "ResourceImageDefault" );

				image = ResourceManager::hostage()
							->createResourceWithParamT<ResourceImageDefault>( resourceType, params );

				image->addImagePath( m_folder + _name );

				ResourceManager::hostage()
					->registerResource( image );
				
				image = ResourceManager::hostage()
					->getResourceT<ResourceImageDefault>( fullname );
			}

			m_mapImageEmitters.insert( std::make_pair( fullname, image ) );

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
