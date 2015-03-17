#	include "ResourceImageSubstractRGBAndAlpha.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/ResourceInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageSubstractRGBAndAlpha );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSubstractRGBAndAlpha::ResourceImageSubstractRGBAndAlpha()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageSubstractRGBAndAlpha::getResourceRGBName() const
    {
        return m_resourceImageRGBName;
    }
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageSubstractRGBAndAlpha::getResourceAlphaName() const
	{
		return m_resourceImageAlphaName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSubstractRGBAndAlpha::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageSubstractRGBAndAlpha *>(_meta);

        m_isAlpha = true;
        m_wrapU = false;
        m_wrapV = false;

        m_resourceImageRGBName = metadata->get_Image_NameRGB();
		m_resourceImageAlphaName = metadata->get_Image_NameAlpha();

		m_uv_image = metadata->get_Image_UVRGB();
		metadata->get_Image_UVRGBRotate( m_isUVRGBRotate );

		m_uv_alpha = metadata->get_Image_UVAlpha();
		metadata->get_Image_UVAlphaRotate( m_isUVAlphaRotate );

		m_maxSize = metadata->get_Image_MaxSize();

		m_size = m_maxSize;
		metadata->get_Image_Size( m_size );
		metadata->get_Image_Offset( m_offset );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSubstractRGBAndAlpha::_compile()
	{	
		if( m_resourceImageRGBName.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageSubstractRGBAndAlpha::_compile '%s' not setup rgb resource"
				, this->getName().c_str()
				);

			return false;
		}

		if( m_resourceImageAlphaName.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageSubstractRGBAndAlpha::_compile '%s' not setup alpha resource"
				, this->getName().c_str()
				);

			return false;
		}
        
		m_resourceImageRGB = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceImage>( m_resourceImageRGBName );

		if( m_resourceImageRGB == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageSubstractRGBAndAlpha::_compile '%s' invalid get rgb resource '%s'"
				, this->getName().c_str()
				, m_resourceImageRGBName.c_str()
				);

			return false;
		}

		m_resourceImageAlpha = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceImage>( m_resourceImageAlphaName );

		if( m_resourceImageAlpha == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageSubstractRGBAndAlpha::_compile '%s' invalid get alpha resource '%s'"
				, this->getName().c_str()
				, m_resourceImageRGBName.c_str()
				);

			return false;
		}

		m_texture = m_resourceImageRGB->getTexture();
		m_textureAlpha = m_resourceImageAlpha->getTexture();

		mt::vec2f uv_size(m_uv_image.z - m_uv_image.x, m_uv_image.w - m_uv_image.y);

		if( m_maxSize.x < 1.f || m_maxSize.y < 1.f )
		{
			const mt::vec2f & maxSize = m_resourceImageRGB->getMaxSize();

			m_maxSize = maxSize * uv_size;

			if( m_isUVRGBRotate == true )
			{
				std::swap( m_maxSize.x, m_maxSize.y );
			}
		}

		if( m_size.x < 1.f || m_size.y < 1.f )
		{
			const mt::vec2f & size = m_resourceImageRGB->getSize();

			m_size = size * uv_size;

			if( m_isUVRGBRotate == true )
			{
				std::swap( m_size.x, m_size.y );
			}
		}
		
		return true;
	}	
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSubstractRGBAndAlpha::_release()
	{
		ResourceImage::_release();

		if( m_resourceImageRGB != nullptr )
		{
			m_resourceImageRGB->decrementReference();
			m_resourceImageRGB = nullptr;
		}

		if( m_resourceImageAlpha != nullptr )
		{
			m_resourceImageAlpha->decrementReference();
			m_resourceImageAlpha = nullptr;
		}
	}
	/////////////////////////////////////////////////////////////////////////
}
