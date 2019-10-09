#include "ResourceImageSubstract.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"

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
    void ResourceImageSubstract::setResourceImageName( const ConstString & _resourceImageName )
    {
        m_resourceImageName = _resourceImageName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceImageSubstract::getResourceImageName() const
    {
        return m_resourceImageName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImageSubstract::_compile()
    {
        if( ResourceImage::_compile() == false )
        {
            LOGGER_ERROR( "'%s' invalid compile base ResourceImage class"
                , this->getName().c_str()
            );

            return false;
        }

        if( m_resourceImageName.empty() == true )
        {
            LOGGER_ERROR( "'%s' not setup image resource"
                , this->getName().c_str()
            );

            return false;
        }

        const ResourceImagePtr & resourceImage = RESOURCE_SERVICE()
            ->getResource( m_resourceImageName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, false, "'%s' group '%s' invalid get image resource '%s'"
            , this->getName().c_str()
            , this->getGroupName().c_str()
            , m_resourceImageName.c_str()
        );

        m_resourceImage = resourceImage;
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

        if( m_resourceImage != nullptr )
        {
            m_resourceImage->release();
            m_resourceImage = nullptr;
        }
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
