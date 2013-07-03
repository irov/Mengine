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
        : m_texture(nullptr)
        , m_textureAlpha(nullptr)
        , m_textureSize(0.f, 0.f)
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
	bool ResourceImage::prepareImageFrame_( const RenderTextureInterfacePtr & texture )
	{
		float width = (float)texture->getWidth();
		float height = (float)texture->getHeight();

		m_size.x = width;
		m_size.y = height;

		const Rect & rect = texture->getRect();

        float hwWidth = (float)texture->getHWWidth();
        float hwHeight = (float)texture->getHWHeight();

        m_uv_scale.x = float(rect.left) / hwWidth;
        m_uv_scale.y = float(rect.top) / hwHeight;
        m_uv_scale.z = float(rect.right) / hwWidth;
        m_uv_scale.w = float(rect.bottom) / hwHeight;

        if( m_maxSize.x < 1.f || m_maxSize.y < 1.f )
        {
            m_maxSize.x = m_size.x;
            m_maxSize.y = m_size.y;
        }

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

        size_t texture_width = (size_t)m_textureSize.x;
        size_t texture_height = (size_t)m_textureSize.y;

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
            ->loadTexture( _pakName, _fileName, _codec, texture_width, texture_height );

		if( texture == nullptr )
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
	ImageDecoderInterfacePtr ResourceImage::createDecoder_(  const InputStreamInterfacePtr & _file, const ConstString& _codec ) const
	{
		ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(m_serviceProvider)
            ->createDecoderT<ImageDecoderInterfacePtr>( _codec, _file );

		if( imageDecoder == nullptr )
		{
			return nullptr;
		}
        
		return imageDecoder;
	}
	//////////////////////////////////////////////////////////////////////////
    void ResourceImage::_release()
    {
        if( m_texture != nullptr )
        {
            const FilePath & filePath = m_texture->getFileName();

            LOGGER_INFO(m_serviceProvider)("ResourceImage::_release %s release texture %s"
                , this->getName().c_str()
                , filePath.c_str()
                );

            m_texture = nullptr;
        }

        if( m_textureAlpha != nullptr )
        {
            const FilePath & filePath = m_textureAlpha->getFileName();

            LOGGER_INFO(m_serviceProvider)("ResourceImage::_release %s release texture alpha %s"
                , this->getName().c_str()
                , filePath.c_str()
                );

            m_textureAlpha = nullptr;
        }
    }
}
