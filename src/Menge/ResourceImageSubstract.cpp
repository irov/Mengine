#	include "ResourceImageSubstract.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/ResourceInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageSubstract );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSubstract::ResourceImageSubstract()
        : m_resourceImage(nullptr)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstract::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_DataBlock::Meta_ResourceImageSubstract * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageSubstract *>(_meta);
        
        m_isAlpha = true;

        m_wrapU = false;
        m_wrapV = false;

        m_resourceImageName = metadata->get_Image_Name();

		m_uv_image = metadata->get_Image_UV();
		m_uv_alpha = m_uv_image;

        metadata->get_Image_Alpha( m_isAlpha );

		m_maxSize = metadata->get_Image_MaxSize();

		m_size = m_maxSize;
		metadata->get_Image_Size( m_size );
		metadata->get_Image_Offset( m_offset );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstract::_compile()
    {
        if( m_resourceImageName.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceImageSubstract::_compile '%s' not setup image resource"
                , this->getName().c_str()
                );

            return false;
        }

        m_resourceImage = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceT<ResourceImage>( m_resourceImageName );

        if( m_resourceImage == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceImageSubstract::_compile '%s' invalid get image resource '%s'"
                , this->getName().c_str()
                , m_resourceImageName.c_str()
                );

            return false;
        }
                       
        m_texture = m_resourceImage->getTexture();
        m_textureAlpha = m_resourceImage->getTextureAlpha();
      
        m_wrapU = false;
        m_wrapV = false;
		        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstract::_release()
    {
        ResourceImage::_release();

        if( m_resourceImage != nullptr )
        {
            m_resourceImage->decrementReference();
            m_resourceImage = nullptr;
        }
    }
}
