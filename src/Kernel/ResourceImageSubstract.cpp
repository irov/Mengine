#include "ResourceImageSubstract.h"

#include "Interface/RenderTextureInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
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
        MENGINE_ASSERTION_FATAL( m_resourceImage == nullptr, "resource image not released" );
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
            LOGGER_ERROR( "resource image substract '%s' group '%s' invalid compile base ResourceImage class"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        if( m_resourceImage->compile() == false )
        {
            LOGGER_ERROR( "resource image substract '%s' invalid compile resource image '%s'"
                , this->getName().c_str()
                , m_resourceImage->getName().c_str()
            );

            return false;
        }

        const RenderTextureInterfacePtr & texture = m_resourceImage->getTexture( 0 );
        this->setTexture( 0, texture );

        const RenderTextureInterfacePtr & textureAlpha = m_resourceImage->getTexture( 1 );
        this->setTexture( 1, textureAlpha );

        bool pow2 = texture->isPow2();
        this->setPow2( pow2 );

        bool premultiply = m_resourceImage->isPremultiply();
        this->setPremultiply( premultiply );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstract::_release()
    {
        ResourceImage::_release();

        this->setTexture( 0, nullptr );
        this->setTexture( 1, nullptr );

        m_resourceImage->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstract::_initialize()
    {
        this->correctUVTexture();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstract::_finalize()
    {
        m_resourceImage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstract::correctUVTexture()
    {
        const mt::uv4f & uvImage = this->getUV( 0 );
        this->setUVTexture( 0, uvImage );

        const mt::uv4f & uvAlpha = this->getUV( 1 );
        this->setUVTexture( 1, uvAlpha );
    }
    //////////////////////////////////////////////////////////////////////////
}
