//#	include "ResourceImageAtlas.h"
//#	include "ResourceImplement.h"
//#	include "Utils/Core/Rect.h"
//#	include "RenderEngine.h"
//#	include "LogEngine.h"
//#	include "Interface/ImageCodecInterface.h"
//
//namespace Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	RESOURCE_IMPLEMENT( ResourceImageAtlas );
//	//////////////////////////////////////////////////////////////////////////
//	ResourceImageAtlas::ResourceImageAtlas()
//		: m_texture(NULL)
//		, m_size(0.f,0.f)
//		, m_pixelFormat(PF_A8R8G8B8)
//	{
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceImageAtlas::loader( const Metabuf::Metadata * _parser )
//	{
//		//ResourceReference::loader( _parser );
//
//		//BIN_SWITCH_ID( _parser )
//		//{
//		//	BIN_CASE_ATTRIBUTE( Protocol::Size_Value, m_size );
//		//}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool ResourceImageAtlas::_compile()
//	{	
//		m_texture = RenderEngine::get()
//			->createTexture( m_size.x, m_size.y, m_pixelFormat );
//
//		if( m_texture == NULL )
//		{
//			MENGE_LOG_ERROR("Texture::ResourceImageAtlas: Invalid create texture");
//			return false;
//		}
//
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceImageAtlas::_release()
//	{
//		RenderEngine::get()
//			->releaseTexture( m_texture );
//
//		m_texture = NULL;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	RenderTextureInterface* ResourceImageAtlas::getTexture() const 
//	{
//		return m_texture;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceImageAtlas::getRectForUV_( Rect& _destRect, const mt::vec4f & _uv )
//	{
//		//float uvX = uv.z - uv.x;
//		//float uvY = uv.w - uv.y;
//		_destRect.left =  _uv.x * m_size.x;
//		_destRect.top =  _uv.y * m_size.y;
//		_destRect.right = _uv.z * m_size.x;
//		_destRect.bottom = _uv.w * m_size.y;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool ResourceImageAtlas::setImageInAtlas( ResourceImage * _resource, ResourceImage::ImageFrame & _frame )
//	{
//		Rect rect;
//
//		this->getRectForUV_( rect, _frame.uv );
//		size_t width = rect.getWidth();
//		size_t height = rect.getHeight();
//
//		int pitch;
//		unsigned char* buffer = m_texture->lock( &pitch, rect, false );
//
//		if( buffer == NULL )
//		{
//			return false;
//		}
//
//		if ( _resource->loadBuffer( buffer, pitch ) == false )
//		{
//			m_texture->unlock();
//			return false;
//		}
//
//		m_texture->unlock();
//
//		_frame.texture = m_texture;
//		_frame.size.x = width;
//		_frame.size.y = height;
//		_frame.maxSize = _frame.size;
//		_frame.uv_image = _frame.uv;
//		_frame.uv_scale = mt::vec4f(0.f, 0.f, 1.0f, 1.0f);
//		_frame.isAlpha = true;
//
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//}
