#	include "ResourceImage.h"

#	include "RenderEngine.h"
#	include "LogEngine.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "LogEngine.h"

#	include "Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ResourceImage()
        : m_texture(0)
        , m_textureAlpha(0)
        , m_maxSize(0.f, 0.f)
        , m_size(0.f, 0.f)
        , m_uv_image(0.f, 0.f, 0.f, 0.f)
        , m_uv(0.f, 0.f, 0.f, 0.f)
        , m_uv_scale(0.f, 0.f, 0.f, 0.f)
        , m_uv_alpha(0.f, 0.f, 0.f, 0.f)
        , m_isAlpha(false)
        , m_wrapX(false)
        , m_wrapY(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::prepareImageFrame_( RenderTextureInterface* texture )
	{
		float width = (float)texture->getWidth();
		float height = (float)texture->getHeight();

		m_size.x = width;
		m_size.y = height;

		const Rect & rect = texture->getRect();
		
        size_t hwWidth = (float)texture->getHWWidth();
        size_t hwHeight = (float)texture->getHWHeight();

        m_maxSize.x = hwWidth;
        m_maxSize.y = hwHeight;

        m_uv_scale.x = float(rect.left) / hwWidth;
        m_uv_scale.y = float(rect.top) / hwHeight;
        m_uv_scale.z = float(rect.right) / hwWidth;
        m_uv_scale.w = float(rect.bottom) / hwHeight;

		m_texture = texture;

		PixelFormat pixelFrormat = texture->getPixelFormat();

		if( pixelFrormat == PF_R8G8B8 )
		{
			m_isAlpha = false;
		}
		else
		{
			m_isAlpha = true;
		}

        float ku = m_uv.z - m_uv.x;
        float kv = m_uv.w - m_uv.y;
        m_size.x *= ku;
        m_size.y *= kv;

        m_uv_image.x = m_uv_scale.x + (m_uv_scale.z - m_uv_scale.x) * m_uv.x;
        m_uv_image.y = m_uv_scale.y + (m_uv_scale.w - m_uv_scale.y) * m_uv.y;
        m_uv_image.z = m_uv_scale.x + (m_uv_scale.z - m_uv_scale.x) * m_uv.z;
        m_uv_image.w = m_uv_scale.y + (m_uv_scale.w - m_uv_scale.y) * m_uv.w;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::loadImageFrame_( const ConstString& _pakName, const WString& _fileName, const ConstString& _codec )
	{
		RenderTextureInterface* texture = RenderEngine::get()
			->loadTexture( _pakName, _fileName, _codec );

		//RenderTextureInterface* texture = RenderEngine::get()
		//	->loadMegatexture( _pakName, _fileName, _codec );

		if( texture == 0 )
		{
			MENGE_LOG_ERROR( "ResourceImage::loadImageFrame_: '%s' can't load image file '%S'"
				, this->getName().c_str()
				, _fileName.c_str()
				);

			return false;
		}

		if( this->prepareImageFrame_( texture ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImage::s_getImageCodec( const WString & _filename )
	{
		WString codecExt;
		Utils::getFileExt( _filename, codecExt );

		const ConstString & codecType = CodecEngine::get()
			->findCodecType( codecExt );

		return codecType;
	}
	//////////////////////////////////////////////////////////////////////////
	ImageDecoderInterface * ResourceImage::createDecoder_(  FileInputStreamInterface * _file, const ConstString& _codec ) const
	{
		ImageDecoderInterface * imageDecoder = CodecEngine::get()
			->createDecoderT<ImageDecoderInterface>( _codec, _file );

		if( imageDecoder == 0 )
		{
			return NULL;
		}

		const ImageCodecDataInfo* dataInfo = imageDecoder->getCodecDataInfo();

		if( dataInfo->format == PF_UNKNOWN )
		{
			imageDecoder->destroy();
			return NULL;
		}

		return imageDecoder;
	}
	//////////////////////////////////////////////////////////////////////////
}
