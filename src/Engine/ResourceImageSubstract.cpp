#include "ResourceImageSubstract.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceInterface.h"

#include "Metacode/Metacode.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSubstract::ResourceImageSubstract()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSubstract::~ResourceImageSubstract()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstract::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSubstract * metadata 
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSubstract *>(_meta);
        
        m_hasAlpha = true;

        m_resourceImageName = metadata->get_Image_Name();

		m_uvImage = metadata->get_Image_UV();
		m_uvAlpha = m_uvImage;

        metadata->get_Image_UVRotate( &m_uvImageRotate );
        m_uvAlphaRotate = m_uvImageRotate;

        metadata->get_Image_Alpha( &m_hasAlpha );

		m_maxSize = metadata->get_Image_MaxSize();

		m_size = m_maxSize;
		metadata->get_Image_Size( &m_size );
		metadata->get_Image_Offset( &m_offset );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstract::_compile()
    {
        if( m_resourceImageName.empty() == true )
        {
            LOGGER_ERROR("ResourceImageSubstract::_compile '%s' not setup image resource"
                , this->getName().c_str()
                );

            return false;
        }

        m_resourceImage = RESOURCE_SERVICE()
            ->getResource( m_resourceImageName );

        if( m_resourceImage == nullptr )
        {
            LOGGER_ERROR("ResourceImageSubstract::_compile '%s' category '%s' group '%s' invalid get image resource '%s'"
                , this->getName().c_str()
				, this->getCategory().c_str()
				, this->getGroup().c_str()
                , m_resourceImageName.c_str()
                );

            return false;
        }
                       
        m_texture = m_resourceImage->getTexture();
        m_textureAlpha = m_resourceImage->getTextureAlpha();
      		        
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
