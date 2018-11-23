#include "ResourceImageSubstractRGBAndAlpha.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSubstractRGBAndAlpha::ResourceImageSubstractRGBAndAlpha()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImageSubstractRGBAndAlpha::~ResourceImageSubstractRGBAndAlpha()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstractRGBAndAlpha::setResourceImageRGBName( const ConstString & _resourceImageRGBName )
    {
        m_resourceImageRGBName = _resourceImageRGBName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageSubstractRGBAndAlpha::getResourceRGBName() const
    {
        return m_resourceImageRGBName;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstractRGBAndAlpha::setResourceImageAlphaName( const ConstString & _resourceImageAlphaName )
    {
        m_resourceImageAlphaName = _resourceImageAlphaName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageSubstractRGBAndAlpha::getResourceAlphaName() const
    {
        return m_resourceImageAlphaName;
    }    
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstractRGBAndAlpha::_compile()
    {
        if( m_resourceImageRGBName.empty() == true )
        {
            LOGGER_ERROR( "ResourceImageSubstractRGBAndAlpha::_compile '%s' not setup rgb resource"
                , this->getName().c_str()
            );

            return false;
        }

        if( m_resourceImageAlphaName.empty() == true )
        {
            LOGGER_ERROR( "ResourceImageSubstractRGBAndAlpha::_compile '%s' not setup alpha resource"
                , this->getName().c_str()
            );

            return false;
        }

        m_resourceImageRGB = RESOURCE_SERVICE()
            ->getResource( m_resourceImageRGBName );

        if( m_resourceImageRGB == nullptr )
        {
            LOGGER_ERROR( "ResourceImageSubstractRGBAndAlpha::_compile '%s' category '%s' group '%s' invalid get rgb resource '%s'"
                , this->getName().c_str()
                , this->getFileGroup()->getName().c_str()
                , this->getGroupName().c_str()
                , m_resourceImageRGBName.c_str()
            );

            return false;
        }

        m_resourceImageAlpha = RESOURCE_SERVICE()
            ->getResource( m_resourceImageAlphaName );

        if( m_resourceImageAlpha == nullptr )
        {
            LOGGER_ERROR( "ResourceImageSubstractRGBAndAlpha::_compile '%s' category '%s' group '%s' invalid get alpha resource '%s'"
                , this->getName().c_str()
                , this->getFileGroup()->getName().c_str()
                , this->getGroupName().c_str()
                , m_resourceImageRGBName.c_str()
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
    //////////////////////////////////////////////////////////////////////////
    void ResourceImageSubstractRGBAndAlpha::correctUVTexture()
    {
        for( uint32_t i = 0; i != 4; ++i )
        {
            m_uvTextureImage[i] = m_uvImage[i];
        }
    }
    /////////////////////////////////////////////////////////////////////////
}
