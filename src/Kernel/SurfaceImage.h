#pragma once

#include "Kernel/Surface.h"
#include "Kernel/ResourceImage.h"

namespace Mengine
{
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

    public:
        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        void correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const override;

        const Color & getColor() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        ResourceImagePtr m_resourceImage;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveSurfacePtr<SurfaceImage> SurfaceImagePtr;
    //////////////////////////////////////////////////////////////////////////
}
