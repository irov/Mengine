#	include "ResourceEmitterContainer.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

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
	void ResourceEmitterContainer::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "File", "Path", m_filename );
			XML_CASE_ATTRIBUTE_NODE( "Folder", "Path", m_folder );
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

		if ( it == m_mapImageEmitters.end() )
		{
			RenderImageInterface * image = Holder<RenderEngine>::hostage()->loadImage( fullname, 1 );

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