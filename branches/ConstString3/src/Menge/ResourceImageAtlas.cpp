#	include "ResourceImageAtlas.h"
#	include "ResourceImplement.h"
#	include "BinParser.h"
#	include "Utils/Core/Rect.h"
#	include "RenderEngine.h"
#	include "Texture.h"
#	include "LogEngine.h"
#	include "Interface/ImageCodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageAtlas );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageAtlas::ResourceImageAtlas()
		: m_texture(NULL)
		, m_size(0.f,0.f)
		, m_pixelFormat(PF_A8R8G8B8)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageAtlas::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Size_Value, m_size );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageAtlas::_compile()
	{	
		m_texture = RenderEngine::get()->createTexture(m_name, m_size.x, m_size.y, m_pixelFormat );
		if( m_texture == NULL )
		{
			MENGE_LOG_ERROR("Texture::ResourceImageAtlas: Invalid create texture");
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageAtlas::_release()
	{
		RenderEngine::get()
			->releaseTexture(m_texture);

		m_texture = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	TextureInterface* ResourceImageAtlas::getTexture() const 
	{
		return m_texture;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageAtlas::_getRectForUV( Rect& _destRect, const mt::vec4f & _uv )
	{
		//float uvX = uv.z - uv.x;
		//float uvY = uv.w - uv.y;
		_destRect.left =  _uv.x * m_size.x;
		_destRect.top =  _uv.y * m_size.y;
		_destRect.right = _uv.z * m_size.x;
		_destRect.bottom = _uv.w * m_size.y;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageAtlas::loadFrame( ImageDecoderInterface * _imageDecoder, ResourceImage::ImageFrame & _frame )
	{
		Rect rect;
		int pitch;

		_getRectForUV( rect, _frame.uv );
		int width = rect.getWidth();
		int height = rect.getHeight();

		unsigned char* buffer = m_texture->lockRect( &pitch, rect, false );
		if( buffer == NULL )
		{
			return false;
		}

		const ImageCodecDataInfo* dataInfo = _imageDecoder->getCodecDataInfo();

		ImageCodecOptions options;

		options.flags |= DF_COUNT_ALPHA;

		if( pitch != width )
		{
			options.pitch = pitch;
			options.flags |= DF_CUSTOM_PITCH;
		}		

		_imageDecoder->setOptions( &options );

		unsigned int bufferSize = pitch * height;
		_imageDecoder->decode( buffer, bufferSize );

		m_texture->unlock();

		_frame.texture = m_texture;
		_frame.size.x = width;
		_frame.size.y = height;
		_frame.maxSize = _frame.size;
		_frame.uv_image = _frame.uv;
		_frame.pow_scale = mt::vec2f( 1.0f, 1.0f );
		_frame.isAlpha = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
