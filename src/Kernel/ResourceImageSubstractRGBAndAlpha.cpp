#include "ResourceImageSubstractRGBAndAlpha.h"

#include "Interface/RenderTextureInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSubstractRGBAndAlpha::ResourceImageSubstractRGBAndAlpha()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSubstractRGBAndAlpha::~ResourceImageSubstractRGBAndAlpha()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImageRGB );
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImageAlpha );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstractRGBAndAlpha::setResourceImageRGB( const ResourceImagePtr & _resourceImageRGB )
    {
        m_resourceImageRGB = _resourceImageRGB;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceImageSubstractRGBAndAlpha::getResourceRGB() const
    {
        return m_resourceImageRGB;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstractRGBAndAlpha::setResourceImageAlpha( const ResourceImagePtr & _resourceImageAlpha )
    {
        m_resourceImageAlpha = _resourceImageAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & ResourceImageSubstractRGBAndAlpha::getResourceAlpha() const
    {
        return m_resourceImageAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstractRGBAndAlpha::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImageRGB, "'%s' group '%s' not setup rgb resource"
            , this->getName().c_str()
            , this->getGroupName().c_str()
        );

        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImageAlpha, "'%s' group '%s' not setup alpha resource"
            , this->getName().c_str()
            , this->getGroupName().c_str()
        );

        if( ResourceImage::_compile() == false )
        {
            LOGGER_ERROR( "'%s' group '%s' invalid compile base ResourceImage class"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        if( m_resourceImageRGB->compile() == false )
        {
            LOGGER_ERROR( "'%s' invalid compile resource image rgb '%s'"
                , this->getName().c_str()
                , m_resourceImageRGB->getName().c_str()
            );

            return false;
        }

        if( m_resourceImageAlpha->compile() == false )
        {
            LOGGER_ERROR( "'%s' invalid compile resource image alpha '%s'"
                , this->getName().c_str()
                , m_resourceImageAlpha->getName().c_str()
            );

            return false;
        }

        m_texture = m_resourceImageRGB->getTexture();
        m_textureAlpha = m_resourceImageAlpha->getTexture();

        bool pow2 = m_texture->isPow2();

        this->setPow2( pow2 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstractRGBAndAlpha::_release()
    {
        ResourceImage::_release();

        m_resourceImageRGB->release();
        m_resourceImageAlpha->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstractRGBAndAlpha::_initialize()
    {
        this->correctUVTexture();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstractRGBAndAlpha::_finalize()
    {
        m_resourceImageRGB = nullptr;
        m_resourceImageAlpha = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstractRGBAndAlpha::correctUVTexture()
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
    /////////////////////////////////////////////////////////////////////////
}
