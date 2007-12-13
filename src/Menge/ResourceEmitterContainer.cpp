#	include "ResourceEmitterContainer.h"

#	include "ResourceImplement.h"

#	include "XmlParser/XmlParser.h"

#	include "RenderEngine.h"

#	include "ParticleEngine.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceEmitterContainer )
	//////////////////////////////////////////////////////////////////////////
	ResourceEmitterContainer::ResourceEmitterContainer( const std::string & _name )
		: ResourceReference( _name )
		, m_container( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceEmitterContainer::loader( TiXmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_VALUE_NODE( "File", "Path", m_filename );
			XML_CHECK_VALUE_NODE( "Folder", "Path", m_folder );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceEmitterContainer::_compile()
	{
		m_container = Holder<ParticleEngine>::hostage()->createEmitterContainerFromFile( m_filename );

		if( m_container == 0 )
		{
			MENGE_LOG( "Error: Image can't create container file '%s'", m_filename.c_str() );
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
			Holder<RenderEngine>::hostage()
				->releaseImage( it->second );
		}
		m_mapImageEmitters.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * ResourceEmitterContainer::getRenderImage( const std::string & _name )
	{
		std::string fullname = m_folder + _name;

		TMapImageEmitters::iterator it = m_mapImageEmitters.find( fullname );

		std::vector<char> buff;

		if ( it == m_mapImageEmitters.end() )
		{
			RenderImageInterface * image = Holder<RenderEngine>::hostage()->loadImage( fullname, buff, 1 );

			if( image == 0 )
			{
				return false;
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