#	include "ResourceImageSubstractRGBAndAlpha.h"

#	include "Interface/ResourceInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSubstractRGBAndAlpha::ResourceImageSubstractRGBAndAlpha()
		: m_resourceImageRGB(nullptr)
		, m_resourceImageAlpha(nullptr)
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

        m_resourceImageRGBName = metadata->get_Image_NameRGB();
		m_resourceImageAlphaName = metadata->get_Image_NameAlpha();

		m_uv_image = metadata->get_Image_UVRGB();
		m_uv_alpha = metadata->get_Image_UVAlpha();

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
			->getResourceT<ResourceImagePtr>( m_resourceImageRGBName );

		if( m_resourceImageRGB == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageSubstractRGBAndAlpha::_compile '%s' category '%s' group '%s' invalid get rgb resource '%s'"
				, this->getName().c_str()
				, this->getCategory().c_str()
				, this->getGroup().c_str()
				, m_resourceImageRGBName.c_str()
				);

			return false;
		}

		m_resourceImageAlpha = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceImagePtr>( m_resourceImageAlphaName );

		if( m_resourceImageAlpha == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceImageSubstractRGBAndAlpha::_compile '%s' category '%s' group '%s' invalid get alpha resource '%s'"
				, this->getName().c_str()
				, this->getCategory().c_str()
				, this->getGroup().c_str()
				, m_resourceImageRGBName.c_str()
				);

			return false;
		}

		m_texture = m_resourceImageRGB->getTexture();
		m_textureAlpha = m_resourceImageAlpha->getTexture();

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
