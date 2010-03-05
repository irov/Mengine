#	include "ResourceEmitterContainer.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

//#	include "RenderEngine.h"
//#	include "Texture.h"

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
	void ResourceEmitterContainer::setFilePath( const String& _path )
	{
		m_filename = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::setFolderPath( const String& _path )
	{
		m_folder = _path;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourceEmitterContainer::getFilePath() const
	{
		return m_filename;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourceEmitterContainer::getFolderPath() const
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
		m_container = Holder<ParticleEngine>::hostage()
						->createEmitterContainerFromFile( m_params.category, m_filename );

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
			//Holder<RenderEngine>::hostage()
			//	->releaseTexture( it->second );
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
	ResourceImageDefault* ResourceEmitterContainer::getRenderImage( const String& _name )
	{
		String fullname = m_params.category + "/" + m_folder + _name;

		TMapImageEmitters::iterator it = m_mapImageEmitters.find( fullname );

		if ( it == m_mapImageEmitters.end() )
		{

			ResourceImageDefault* image = Holder<ResourceManager>::hostage()
											->getResourceT<ResourceImageDefault>( fullname );

			if( image == 0 )
			{
				ResourceFactoryParam params 
					= { fullname, m_params.category, m_params.group, m_params.file };
				image = Holder<ResourceManager>::hostage()
							->createResourceWithParamT<ResourceImageDefault>( "ResourceImageDefault", params );
				image->addImagePath( m_folder + _name );
				Holder<ResourceManager>::hostage()
					->registerResource( image );
				
				image = Holder<ResourceManager>::hostage()
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
