#	include "ResourceImageDynamic.h"

#	include "ResourceImplement.h"

#	include "RenderEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageDynamic );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDynamic::ResourceImageDynamic( const std::string & _name )
		: ResourceImage( _name )
		, m_offset( 0.0f, 0.0f )
		, m_uv( 0.0f, 0.0f, 1.0f, 1.0f )
	{
		m_frame.image = NULL;
		m_frame.size = mt::vec2f::zero_v2;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getMaxSize( unsigned int _frame ) const
	{
		return m_frame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ResourceImageDynamic::getCount() const
	{
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getSize( unsigned int _frame ) const
	{
		return m_frame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getOffset( unsigned int _frame ) const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDynamic::getUV( unsigned int _frame ) const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageDynamic::getImage( unsigned int _frame ) const
	{
		return m_frame.image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* ResourceImageDynamic::getImage( unsigned int _frame )
	{
		return m_frame.image;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::loader( XmlElement * _xml )
	{
		ResourceImage::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::setRenderImage( RenderImageInterface * _image )
	{
		m_frame.image = _image;
		m_frame.size = mt::vec2f( _image->getWidth(), _image->getHeight() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDynamic::_compile()
	{	
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::_release()
	{
		//releaseImageFrame( m_frame );
	}
	//////////////////////////////////////////////////////////////////////////
}