#	include "ResourceImageVideoStream.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageVideoStream )
		//////////////////////////////////////////////////////////////////////////
		ResourceImageVideoStream::ResourceImageVideoStream( const std::string & _name )
		: ResourceImage( _name )
		, m_filename("")
		, m_uv( 0.0f, 0.0f, 1.0f, 1.0f )
		, m_offset( 0.0f, 0.0f )
		, m_size( 0.0f, 0.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageVideoStream::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "File", "Path", m_filename );
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
				, m_name.c_str()
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
	size_t ResourceImageVideoStream::getCount() const
	{
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageVideoStream::getSize( size_t _frame ) const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& ResourceImageVideoStream::getMaxSize( size_t _frame ) const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageVideoStream::getOffset( size_t _frame ) const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageVideoStream::getUV( size_t _frame ) const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageVideoStream::getImage( size_t _frame ) const
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* ResourceImageVideoStream::getImage( size_t _frame )
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVideoStreamInterface* ResourceImageVideoStream::getStream()
	{
		return m_interface;
	}
}