#	include "ResourceImageSubstract.h"
#	include "ResourceImplement.h"

#	include "ResourceManager.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"

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
    void ResourceImageSubstract::loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_DataBlock::Meta_ResourceImageSubstract * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageSubstract *>(_meta);
        
        metadata->swap_Image_Name( m_resourceImageName );
        m_uv = metadata->get_Image_UV();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstract::_compile()
    {
        if( m_resourceImageName.empty() == true )
        {
            return false;
        }

        m_resourceImage = ResourceManager::get()
            ->getResourceT<ResourceImage>( m_resourceImageName );

        if( m_resourceImage == NULL )
        {
            return false;
        }
        
        bool wrapX = m_resourceImage->getWrapX();

        if( wrapX == true )
        {
            return false;
        }

        bool wrapY = m_resourceImage->getWrapY();

        if( wrapY == true )
        {
            return false;
        }

        m_texture = m_resourceImage->getTexture();
        m_textureAlpha = m_resourceImage->getTextureAlpha();

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
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstract::_release()
    {
        m_resourceImage->decrementReference();
        m_resourceImage = NULL;
    }
}
