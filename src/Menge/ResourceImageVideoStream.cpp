#	include "ResourceImageVideoStream.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageVideoStream );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageVideoStream::ResourceImageVideoStream( const ResourceFactoryParam & _params )
		: ResourceImage( _params )
		, m_uv( 0.0f, 0.0f, 1.0f, 1.0f )
		, m_offset( 0.0f, 0.0f )
		, m_size( 0.0f, 0.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageVideoStream::setFilePath( const std::string & _path )
	{
		m_filename = m_params.category + _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageVideoStream::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "File", "Path", &ResourceImageVideoStream::setFilePath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageVideoStream::_compile()
	{
		m_interface = 
			Holder<RenderEngine>::hostage()
			->loadImageVideoStream( m_filename );

		if( m_interface == 0 )
		{
			MENGE_LOG("Warning: resource '%s' can't load video file '%s'\n"
				, getName().c_str()
				, m_filename.c_str()
				);

			return false;
		}

		m_size.x = m_interface->getWidth();
		m_size.y = m_interface->getHeight();

		return true;	
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageVideoStream::_release()
	{
		Holder<RenderEngine>::hostage()
			->releaseImageVideoStream( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ResourceImageVideoStream::getCount() const
	{
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageVideoStream::getSize( unsigned int _frame ) const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceImageVideoStream::getMaxSize( unsigned int _frame ) const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageVideoStream::getOffset( unsigned int _frame ) const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageVideoStream::getUV( unsigned int _frame ) const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageVideoStream::isAlpha( unsigned int _frame ) const
	{
		assert(!"ResourceImageVideoStream::isAlpha not implemented");
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageVideoStream::getImage( unsigned int _frame ) const
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* ResourceImageVideoStream::getImage( unsigned int _frame )
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVideoStreamInterface* ResourceImageVideoStream::getStream()
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
}