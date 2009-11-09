#	include "ResourceImageDynamic.h"

#	include "ResourceImplement.h"

#	include "RenderEngine.h"
#	include "FileEngine.h"
#	include "Texture.h"
#	include "Utils.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageDynamic );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDynamic::ResourceImageDynamic()
		: m_offset( 0.0f, 0.0f )
		, m_uv( 0.0f, 0.0f, 1.0f, 1.0f )
		, m_cached( false )
	{
		m_frame.texture = NULL;
		m_frame.size = mt::vec2f::zero_v2;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDynamic::~ResourceImageDynamic()
	{
		if( m_cached == true )
		{
			String cashName = m_params.category + m_params.group + "cache_" + m_params.name + ".png";
			Holder<FileEngine>::hostage()
				->removeFile( m_params.category, cashName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getMaxSize( std::size_t _frame ) const
	{
		return m_frame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageDynamic::getCount() const
	{
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getSize( std::size_t _frame ) const
	{
		return m_frame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getOffset( std::size_t _frame ) const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDynamic::getUV( std::size_t _frame ) const
	{
		return m_uv;
	}
	/////////////////////////////////////////////////////////////////////////
	const String & ResourceImageDynamic::getFilename( std::size_t _frame ) const
	{
		assert(!"ResourceImageDynamic::getFilename not implemented");
		return Utils::emptyString();
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageDynamic::getFilenameCount() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDynamic::isAlpha( std::size_t _frame ) const
	{
		//assert(!"ResourceImageDynamic::isAlpha not implemented");
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceImageDynamic::getTexture( std::size_t _frame )
	{
		return m_frame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::loader( XmlElement * _xml )
	{
		ResourceImage::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::setRenderImage( Texture* _image )
	{
		m_frame.texture = _image;
		m_frame.size.x = (float)_image->getWidth();
		m_frame.size.y = (float)_image->getHeight();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDynamic::_compile()
	{	
		if( m_cached )
		{
			String cashName =  m_params.group + "cache_" + m_params.name + ".png";
			m_frame = loadImageFrame( m_params.category, cashName );
		}
		m_cached = false;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::_release()
	{
		if( m_frame.texture != 0 )
		{
			String cashName = m_params.group + "cache_" + m_params.name + ".png";
			Holder<RenderEngine>::hostage()
				->saveImage( m_frame.texture, m_params.category, cashName );

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
