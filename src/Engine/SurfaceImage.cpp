#include "SurfaceImage.h"

#include "Interface/RenderSystemInterface.h"

#include "Kernel/ResourceImage.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
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

        this->recompile( [this, _resourceImage]() {m_resourceImage = _resourceImage; } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & SurfaceImage::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImage::_compile()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, false, "'%s' resource is null"
            , this->getName().c_str()
        );

        if( m_resourceImage.compile() == false )
        {
            LOGGER_ERROR( "'%s' resource '%s' is not compile"
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
        m_resourceImage.release();

        this->releaseMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SurfaceImage::_update( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImage::getMaxSize() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return mt::vec2f::identity();
        }

        const mt::vec2f & maxSize = m_resourceImage->getMaxSize();

        return maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImage::getSize() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, mt::vec2f::identity(), "'%s' not setup resource"
            , this->getName().c_str()
        );

        const mt::vec2f & size = m_resourceImage->getSize();

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SurfaceImage::getOffset() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, mt::vec2f::identity(), "'%s' not setup resource"
            , this->getName().c_str()
        );

        const mt::vec2f & offset = m_resourceImage->getOffset();

        return offset;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t SurfaceImage::getUVCount() const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "'%s' not compile"
                , this->getName().c_str()
            );

            return 0;
        }

        const RenderTextureInterfacePtr & texture = m_resourceImage->getTexture();

        if( texture == nullptr )
        {
            return 0;
        }

        const RenderTextureInterfacePtr & textureAlpha = m_resourceImage->getTextureAlpha();

        if( textureAlpha == nullptr )
        {
            return 1;
        }

        return 2;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & SurfaceImage::getUV( uint32_t _index ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, mt::uv4f::identity(), "'%s' not setup texture"
            , this->getName().c_str()
        );

        switch( _index )
        {
        case 0:
            {
                const mt::uv4f & uv = m_resourceImage->getUVTextureImage();

                return uv;
            } break;
        case 1:
            {
                const mt::uv4f & uv = m_resourceImage->getUVTextureAlpha();

                return uv;
            } break;
        default:
            {
            }break;
        }

        return mt::uv4f::identity();
    }
    //////////////////////////////////////////////////////////////////////////
    void SurfaceImage::correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * _out )
    {
        MENGINE_ASSERTION_MEMORY_PANIC_VOID( m_resourceImage, "'%s' not setup texture"
            , this->getName().c_str()
        );

        switch( _index )
        {
        case 0:
            {
                m_resourceImage->correctUVImage( _in, _out );
            } break;
        case 1:
            {
                m_resourceImage->correctUVAlpha( _in, _out );
            } break;
        default:
            {
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & SurfaceImage::getColor() const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_resourceImage, Color::identity(), "surface image '%s' not setup texture"
            , this->getName().c_str()
        );

        const Color & color = m_resourceImage->getColor();

        return color;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr SurfaceImage::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = this->makeImageMaterial( m_resourceImage, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( material, nullptr, "'%s' resource '%s' m_material is NULL"
            , this->getName().c_str()
            , m_resourceImage->getName().c_str()
        );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
}
