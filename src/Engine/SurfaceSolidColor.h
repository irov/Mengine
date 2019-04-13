#pragma once

#include "Kernel/Surface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SurfaceSolidColor
        : public Surface
    {
    public:
        SurfaceSolidColor();
        ~SurfaceSolidColor() override;

    public:
        void setSolidColor( const Color & _color );
        const Color & getSolidColor() const;

        void setSolidSize( const mt::vec2f & _size );
        const mt::vec2f & getSolidSize() const;

    public:
        const mt::vec2f & getMaxSize() const override;
        const mt::vec2f & getSize() const override;
        const mt::vec2f & getOffset() const override;

        uint32_t getUVCount() const override;
        const mt::uv4f & getUV( uint32_t _index ) const override;

        const Color & getColor() const override;

        void correctUV( uint32_t _index, const mt::vec2f & _in, mt::vec2f * _out ) override;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        Color m_color;
        mt::vec2f m_size;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SurfaceSolidColor> SurfaceSolidColorPtr;
}
