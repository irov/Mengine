#	include "ResourceEmitterContainer.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

//#	include "RenderEngine.h"
#	include "ResourceManager.h"
#	include "ResourceImageDefault.h"

#	include "ParticleEngine.h"

#	include "LogEngine.h"
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceEmitterContainer );
	//////////////////////////////////////////////////////////////////////////
	ResourceEmitterContainer::ResourceEmitterContainer( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
		, m_container( 0 )
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
		m_container = Holder<ParticleEngine>::hostage()->createEmitterContainerFromFile( m_params.category + m_filename );

		if( m_container == 0 )
		{
			MENGE_LOG_ERROR( "Image can't create container file \"%s\""
				, m_filename.c_str() );
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
			//	->releaseImage( it->second );
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
	ResourceImage* ResourceEmitterContainer::getRenderImage( const String& _name )
	{
		String fullname = m_params.category + m_folder + _name;

		TMapImageEmitters::iterator it = m_mapImageEmitters.find( fullname );
	

		if ( it == m_mapImageEmitters.end() )
		{
			ResourceFactoryParam param;
			param.category = m_params.category + m_folder;
			param.name = fullname;
			ResourceImageDefault* resource = new ResourceImageDefault( param );
			resource->addImagePath( _name );

			Holder<ResourceManager>::hostage()->registerResource( resource );

			ResourceImage* image = Holder<ResourceManager>::hostage()
											->getResourceT<ResourceImage>( fullname );
			//RenderImageInterface * image = Holder<RenderEngine>::hostage()->loadImage( fullname );

			//if( image == 0 )
			//{
			//	return false;
			//}

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
