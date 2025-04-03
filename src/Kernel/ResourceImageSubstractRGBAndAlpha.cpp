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
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImageRGB, "not deallocated rgb resource" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImageAlpha, "not deallocated alpha resource" );
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
            LOGGER_ERROR( "resource image substract RGB and alpha '%s' group '%s' invalid compile base ResourceImage class"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        if( m_resourceImageRGB->compile() == false )
        {
            LOGGER_ERROR( "resource image substract RGB and alpha '%s' invalid compile resource image rgb '%s'"
                , this->getName().c_str()
                , m_resourceImageRGB->getName().c_str()
            );

            return false;
        }

        if( m_resourceImageAlpha->compile() == false )
        {
            LOGGER_ERROR( "resource image substract RGB and alpha '%s' invalid compile resource image alpha '%s'"
                , this->getName().c_str()
                , m_resourceImageAlpha->getName().c_str()
            );

            return false;
        }

        const RenderTextureInterfacePtr & texture = m_resourceImageRGB->getTexture( 0 );
        this->setTexture( 0, texture );

        const RenderTextureInterfacePtr & textureAlpha = m_resourceImageAlpha->getTexture( 0 );
        this->setTexture( 1, textureAlpha );

        bool pow2 = texture->isPow2();
        this->setPow2( pow2 );

        bool premultiply = m_resourceImageRGB->isPremultiply();
        this->setPremultiply( premultiply );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstractRGBAndAlpha::_release()
    {
        ResourceImage::_release();

        this->setTexture( 0, nullptr );
        this->setTexture( 1, nullptr );

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
        const mt::uv4f & uvImage = this->getUV( 0 );
        this->setUVTexture( 0, uvImage );

        const mt::uv4f & uvAlpha = this->getUV( 1 );
        this->setUVTexture( 1, uvAlpha );
    }
    /////////////////////////////////////////////////////////////////////////
}
