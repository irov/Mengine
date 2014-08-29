#	include "ResourceImage.h"

#	include "Interface/ImageCodecInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/CodecInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::ResourceImage()
        : m_texture(nullptr)
        , m_textureAlpha(nullptr)
        , m_maxSize(0.f, 0.f)
        , m_size(0.f, 0.f)
		, m_offset(0.f, 0.f)
        , m_uv_image(0.f, 0.f, 0.f, 0.f)
        , m_uv(0.f, 0.f, 0.f, 0.f)
        , m_uv_scale(0.f, 0.f, 0.f, 0.f)
        , m_uv_alpha(0.f, 0.f, 0.f, 0.f)
        , m_isAlpha(false)
        , m_isUVRotate(false)
        , m_wrapU(false)
        , m_wrapV(false)        
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::~ResourceImage()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::prepareImageFrame_( const RenderTextureInterfacePtr & texture )
	{
		float width = (float)texture->getWidth();
		float height = (float)texture->getHeight();

		const Rect & rect = texture->getRect();

		const RenderImageInterfacePtr & image = texture->getImage();

        float hwWidth = (float)image->getHWWidth();
        float hwHeight = (float)image->getHWHeight();

        m_uv_scale.x = float(rect.left) / hwWidth;
        m_uv_scale.y = float(rect.top) / hwHeight;
        m_uv_scale.z = float(rect.right) / hwWidth;
        m_uv_scale.w = float(rect.bottom) / hwHeight;

        if( m_maxSize.x < 1.f || m_maxSize.y < 1.f )
        {
            m_maxSize.x = width;
            m_maxSize.y = height;
        }

		if( m_size.x < 1.f || m_size.y < 1.f )
		{
			m_size.x = width;
			m_size.y = height;

			float ku = m_uv.z - m_uv.x;
			float kv = m_uv.w - m_uv.y;

			m_size.x *= ku;
			m_size.y *= kv;

			if( m_isUVRotate == true )
			{
				std::swap( m_size.x, m_size.y );
			}
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

        m_uv_image.x = m_uv_scale.x + (m_uv_scale.z - m_uv_scale.x) * m_uv.x;
        m_uv_image.y = m_uv_scale.y + (m_uv_scale.w - m_uv_scale.y) * m_uv.y;
        m_uv_image.z = m_uv_scale.x + (m_uv_scale.z - m_uv_scale.x) * m_uv.z;
        m_uv_image.w = m_uv_scale.y + (m_uv_scale.w - m_uv_scale.y) * m_uv.w;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImage::loadImageFrame_( const ConstString& _pakName, const FilePath& _fileName, const ConstString& _codecType )
	{
        LOGGER_INFO(m_serviceProvider)("ResourceImage::loadImageFrame_ %s load texture %s"
            , this->getName().c_str()
            , _fileName.c_str()
            );

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE(m_serviceProvider)
            ->loadTexture( _pakName, _fileName, _codecType );

		if( texture == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImage::loadImageFrame_: '%s' can't load image file '%s'"
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
	/////////////////////////////////////////////////////////////////////////
	size_t ResourceImage::memoryUse() const
	{
		size_t memory = ResourceReference::memoryUse();

		if( m_texture != nullptr )
		{
			memory += m_texture->getMemoryUse();
		}

		if( m_textureAlpha != nullptr )
		{
			memory += m_textureAlpha->getMemoryUse();
		}

		return memory;
	}
}
