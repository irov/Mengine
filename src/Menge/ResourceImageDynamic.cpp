#	include "ResourceImageDynamic.h"

#	include "ResourceImplement.h"

#	include "RenderEngine.h"
#	include "Image.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageDynamic );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDynamic::ResourceImageDynamic( const ResourceFactoryParam & _params )
		: ResourceImage( _params )
		, m_offset( 0.0f, 0.0f )
		, m_uv( 0.0f, 0.0f, 1.0f, 1.0f )
		, m_cached( false )
	{
		m_frame.image = NULL;
		m_frame.size = mt::vec2f::zero_v2;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDynamic::~ResourceImageDynamic()
	{
		if( m_cached == true )
		{
			String cashName = m_params.category + m_params.group + "cache_" + m_params.name + ".png";
			Holder<FileEngine>::hostage()->deleteFile( cashName );
		}
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
	bool ResourceImageDynamic::isAlpha( unsigned int _frame ) const
	{
		//assert(!"ResourceImageDynamic::isAlpha not implemented");
		return true;
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
		m_frame.size.x = (float)_image->getWidth();
		m_frame.size.y = (float)_image->getHeight();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDynamic::_compile()
	{	
		if( m_cached )
		{
			String cashName = m_params.category + m_params.group + "cache_" + m_params.name + ".png";
			m_frame = loadImageFrame( cashName );
		}
		m_cached = false;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::_release()
	{
		if( m_frame.image != 0 )
		{
			Image wImage;

			unsigned char * buffer = m_frame.image->lock();
			std::size_t width = m_frame.image->getWidth();
			std::size_t height = m_frame.image->getHeight();
			PixelFormat pixelFormat = m_frame.image->getPixelFormat();

			wImage.loadDynamicImage( buffer, width, height, 1, pixelFormat );
			String cashName = m_params.group + "cache_" + m_params.name + ".png";
			wImage.save( cashName );
			m_frame.image->unlock();

			m_cached = true;
			releaseImageFrame( m_frame );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::setSize( const mt::vec2f& _size )
	{
		m_frame.size = _size;
	}
	//////////////////////////////////////////////////////////////////////////
}