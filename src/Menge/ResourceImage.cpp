#	include "ResourceImage.h"

#	include "Interface/ImageCodecInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/CodecInterface.h"

#	include "Logger/Logger.h"

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
        , m_isUVRotate(false)
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
		
        float hwWidth = (float)texture->getHWWidth();
        float hwHeight = (float)texture->getHWHeight();

        m_maxSize.x = hwWidth;
        m_maxSize.y = hwHeight;

        m_uv_scale.x = float(rect.left) / hwWidth;
        m_uv_scale.y = float(rect.top) / hwHeight;
        m_uv_scale.z = float(rect.right) / hwWidth;
        m_uv_scale.w = float(rect.bottom) / hwHeight;

		m_texture = texture;

		size_t channels = texture->getChannels();

		if( channels == 3 )
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
        
        if( m_isUVRotate == true )
        {
            std::swap( m_maxSize.x, m_maxSize.y );
            std::swap( m_size.x, m_size.y );
        }

        m_uv_image.x = m_uv_scale.x + (m_uv_scale.z - m_uv_scale.x) * m_uv.x;
        m_uv_image.y = m_uv_scale.y + (m_uv_scale.w - m_uv_scale.y) * m_uv.y;
        m_uv_image.z = m_uv_scale.x + (m_uv_scale.z - m_uv_scale.x) * m_uv.z;
        m_uv_image.w = m_uv_scale.y + (m_uv_scale.w - m_uv_scale.y) * m_uv.w;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::loadImageFrame_( const ConstString& _pakName, const FilePath& _fileName, const ConstString& _codec )
	{
        LOGGER_INFO(m_serviceProvider)("ResourceImage::loadImageFrame_ %s load texture %s"
            , this->getName().c_str()
            , _fileName.c_str()
            );

		RenderTextureInterface* texture = RENDER_SERVICE(m_serviceProvider)
            ->loadTexture( _pakName, _fileName, _codec );

		//RenderTextureInterface* texture = RenderEngine::get()
		//	->loadMegatexture( _pakName, _fileName, _codec );

		if( texture == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "ResourceImage::loadImageFrame_: '%s' can't load image file '%s'"
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
	ImageDecoderInterface * ResourceImage::createDecoder_(  InputStreamInterface * _file, const ConstString& _codec ) const
	{
		ImageDecoderInterface * imageDecoder = 
            CODEC_SERVICE(m_serviceProvider)->createDecoderT<ImageDecoderInterface>( _codec, _file );

		if( imageDecoder == 0 )
		{
			return NULL;
		}
        
		return imageDecoder;
	}
	//////////////////////////////////////////////////////////////////////////
    void ResourceImage::_release()
    {
        if( m_texture != NULL )
        {
            const FilePath & filePath = m_texture->getFileName();

            LOGGER_INFO(m_serviceProvider)("ResourceImageDefault::_release %s release texture %s"
                , this->getName().c_str()
                , filePath.c_str()
                );

            RENDER_SERVICE(m_serviceProvider)->releaseTexture( m_texture );

            m_texture = NULL;
        }

        if( m_textureAlpha != NULL )
        {
            const FilePath & filePath = m_textureAlpha->getFileName();

            LOGGER_INFO(m_serviceProvider)("ResourceImageDefault::_release %s release texture alpha %s"
                , this->getName().c_str()
                , filePath.c_str()
                );

            RENDER_SERVICE(m_serviceProvider)->releaseTexture( m_textureAlpha );

            m_textureAlpha = NULL;
        }
    }
}
