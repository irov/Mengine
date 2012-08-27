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
        , m_uv(0.f, 0.f, 1.f, 1.f)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstract::loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_DataBlock::Meta_ResourceImageSubstract * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceImageSubstract *>(_meta);
        
        metadata->swap_Image_Name( m_resourceImageName );
        m_uv = metadata->get_File_UV();
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
        
        const ImageFrame & imageFrame = m_resourceImage->getImageFrame();

        m_imageFrame.texture = imageFrame.texture;

        mt::vec2f uv_size(m_uv.z - m_uv.x, m_uv.w - m_uv.y);

        m_imageFrame.maxSize = imageFrame.maxSize * uv_size;
        m_imageFrame.size = imageFrame.maxSize * uv_size;
        m_imageFrame.uv_image = m_uv;
        m_imageFrame.uv = m_uv;
        m_imageFrame.uv_scale = imageFrame.uv_scale;

        m_imageFrame.isAlpha = imageFrame.isAlpha;
        m_imageFrame.wrapX = false;
        m_imageFrame.wrapY = false;

        if( imageFrame.wrapX == true || imageFrame.wrapY == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstract::_release()
    {
        m_resourceImage->decrementReference();
        m_resourceImage = NULL;
    }
}
