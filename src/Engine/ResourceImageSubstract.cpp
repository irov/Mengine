#include "ResourceImageSubstract.h"

#include "Interface/RenderTextureInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

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
    void ResourceImageSubstract::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        m_resourceImage = _resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceImageSubstract::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstract::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "'%s' group '%s' invalid setup image resource"
            , this->getName().c_str()
            , this->getGroupName().c_str()
        );

        if( ResourceImage::_compile() == false )
        {
            LOGGER_ERROR( "'%s' invalid compile base ResourceImage class"
                , this->getName().c_str()
            );

            return false;
        }

        if( m_resourceImage->compile() == false )
        {
            LOGGER_ERROR( "'%s' invalid compile resource image '%s'"
                , this->getName().c_str()
                , m_resourceImage->getName().c_str()
            );

            return false;
        }

        m_texture = m_resourceImage->getTexture();
        m_textureAlpha = m_resourceImage->getTextureAlpha();

        bool pow2 = m_texture->isPow2();

        this->setPow2( pow2 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstract::_release()
    {
        ResourceImage::_release();

        m_resourceImage->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstract::correctUVTexture()
    {
        for( uint32_t i = 0; i != 4; ++i )
        {
            m_uvTextureImage[i] = m_uvImage[i];
        }

        for( uint32_t i = 0; i != 4; ++i )
        {
            m_uvTextureAlpha[i] = m_uvAlpha[i];
        }
    }
}
