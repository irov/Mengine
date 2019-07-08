#pragma once

#include "Kernel/Surface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ResourceImage, class Resource> ResourceImagePtr;
    //////////////////////////////////////////////////////////////////////////
    class SurfaceImage
        : public Surface
    {
    public:
        SurfaceImage();
        ~SurfaceImage() override;

    public:
        void setResourceImage( const ResourceImagePtr & _resourceImage );
        const ResourceImagePtr & getResourceImage() const;

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        void correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * _out ) override;

        const Color & getColor() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool update( const UpdateContext * _context ) override;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        ResourceImagePtr m_resourceImage;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SurfaceImage> SurfaceImagePtr;
}
