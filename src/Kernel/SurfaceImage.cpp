#include "SurfaceImage.h"

#include "Kernel/ResourceImage.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SurfaceImage::SurfaceImage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SurfaceImage::~SurfaceImage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImage::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        if( m_resourceImage == _resourceImage )
        {
            return;
        }

        this->recompile( [this, &_resourceImage]()
        {
            m_resourceImage = _resourceImage;

            if( m_resourceImage == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & SurfaceImage::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImage::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "'%s' resource is nullptr"
            , this->getName().c_str()
        );

        if( m_resourceImage->compile() == false )
        {
            LOGGER_ERROR( "surface image '%s' resource '%s' is not compile"
                , this->getName().c_str()
                , m_resourceImage->getName().c_str()
            );

            return false;
        }

        this->invalidateMaterial();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImage::_release()
    {
        m_resourceImage->release();

        this->releaseMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImage::getMaxSize() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "'%s' not setup resource"
            , this->getName().c_str()
        );

        const mt::vec2f & maxSize = m_resourceImage->getMaxSize();

        return maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImage::getSize() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "'%s' not setup resource"
            , this->getName().c_str()
        );

        const mt::vec2f & size = m_resourceImage->getSize();

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImage::getOffset() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "'%s' not setup resource"
            , this->getName().c_str()
        );

        const mt::vec2f & offset = m_resourceImage->getOffset();

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceImage::getUVCount() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "'%s' not setup resource"
            , this->getName().c_str()
        );

        const RenderTextureInterfacePtr & texture = m_resourceImage->getTexture( 0 );

        if( texture == nullptr )
        {
            return 0;
        }

        const RenderTextureInterfacePtr & textureAlpha = m_resourceImage->getTexture( 1 );

        if( textureAlpha == nullptr )
        {
            return 1;
        }

        return 2;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & SurfaceImage::getUV( uint32_t _index ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "'%s' not setup texture"
            , this->getName().c_str()
        );

        const mt::uv4f & uv = m_resourceImage->getUVTexture( _index );

        return uv;
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & SurfaceImage::getColor() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "surface image '%s' not setup texture"
            , this->getName().c_str()
        );

        const Color & color = m_resourceImage->getColor();

        return color;
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImage::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, "'%s' not setup texture"
            , this->getName().c_str()
        );

        m_resourceImage->correctUV( _index, _in, _out );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr SurfaceImage::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = this->makeImageMaterial( m_resourceImage, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material, "'%s' resource '%s' m_material is nullptr"
            , this->getName().c_str()
            , m_resourceImage->getName().c_str()
        );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
}
