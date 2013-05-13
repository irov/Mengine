#	include "ResourceImageSubstract.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/ResourceInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/CodecInterface.h"

#	include "Logger/Logger.h"


#   include "Metacode.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageSubstract );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSubstract::ResourceImageSubstract()
        : m_resourceImage(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstract::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_DataBlock::Meta_ResourceImageSubstract * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageSubstract *>(_meta);
        
        m_uv = mt::vec4f(0.f, 0.f, 1.f, 1.f);
        m_maxSize = mt::vec2f(-1.f, -1.f);
        m_size = mt::vec2f(-1.f, -1.f);
        m_isAlpha = true;
        m_isUVRotate = false;
        m_wrapX = false;
        m_wrapY = false;

        metadata->swap_Image_Name( m_resourceImageName );
        m_uv = metadata->get_Image_UV();

        metadata->get_Image_Rotate( m_isUVRotate );

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

        if( m_resourceImage == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("ResourceImageSubstract::_compile '%s' invalid get image resource '%s'"
                , this->getName().c_str()
                , m_resourceImageName.c_str()
                );

            return false;
        }
                       
        m_texture = m_resourceImage->getTexture();
        m_textureAlpha = m_resourceImage->getTextureAlpha();


        //ToDo Fix Me isUVRotate
        mt::vec2f uv_size(m_uv.z - m_uv.x, m_uv.w - m_uv.y);

        const mt::vec2f & maxSize = m_resourceImage->getMaxSize();

        m_maxSize = maxSize * uv_size;
        m_size = maxSize * uv_size;
        m_uv_image = m_uv;

        m_uv_scale = m_resourceImage->getUVScale();
        m_uv_alpha = m_resourceImage->getUVAlpha();

        m_isAlpha = m_resourceImage->isAlpha();

        m_wrapX = false;
        m_wrapY = false;

        if( m_isUVRotate == true )
        {
            std::swap( m_maxSize.x, m_maxSize.y );
            std::swap( m_size.x, m_size.y );
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstract::_release()
    {
        if( m_resourceImage != NULL )
        {
            m_resourceImage->decrementReference();
            m_resourceImage = NULL;
        }
    }
}
