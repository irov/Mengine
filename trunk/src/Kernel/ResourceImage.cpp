#	include "ResourceImage.h"

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
		, m_uv_image(0.f, 0.f, 1.f, 1.f)
        , m_uv_alpha(0.f, 0.f, 1.f, 1.f)
        , m_isAlpha(false)
        , m_isUVRGBRotate(false)
		, m_isUVAlphaRotate(false)
        , m_wrapU(false)
        , m_wrapV(false)        
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage::~ResourceImage()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImage::prepareImageFrame_( const RenderTextureInterfacePtr & _texture )
	{
		m_texture = _texture;

		float width = (float)m_texture->getWidth();
		float height = (float)m_texture->getHeight();

		const Rect & rect = m_texture->getRect();

		const RenderImageInterfacePtr & image = m_texture->getImage();

		float hwWidth = (float)image->getHWWidth();
		float hwHeight = (float)image->getHWHeight();

		mt::vec4f uv_scale;
		uv_scale.x = float(rect.left) / hwWidth;
		uv_scale.y = float(rect.top) / hwHeight;
		uv_scale.z = float(rect.right) / hwWidth;
		uv_scale.w = float(rect.bottom) / hwHeight;

		if( m_maxSize.x < 1.f || m_maxSize.y < 1.f )
		{
			m_maxSize.x = width;
			m_maxSize.y = height;
		}

		mt::vec4f m_uv(0.f, 0.f, 1.f, 1.f);

		if( m_size.x < 1.f || m_size.y < 1.f )
		{
			m_size.x = m_maxSize.x;
			m_size.y = m_maxSize.y;

			float ku = m_uv.z - m_uv.x;
			float kv = m_uv.w - m_uv.y;

			m_size.x *= ku;
			m_size.y *= kv;

			if( m_isUVRGBRotate == true )
			{
				std::swap( m_size.x, m_size.y );
			}
		}
		
		uint32_t channels = m_texture->getChannels();

		if( channels == 3 )
		{
			m_isAlpha = false;
		}
		else
		{
			m_isAlpha = true;
		}

		m_uv_image.x = uv_scale.x + (uv_scale.z - uv_scale.x) * m_uv.x;
		m_uv_image.y = uv_scale.y + (uv_scale.w - uv_scale.y) * m_uv.y;
		m_uv_image.z = uv_scale.x + (uv_scale.z - uv_scale.x) * m_uv.z;
		m_uv_image.w = uv_scale.y + (uv_scale.w - uv_scale.y) * m_uv.w;
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
