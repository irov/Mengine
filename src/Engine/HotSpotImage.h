#pragma once

#include "Kernel/HotSpot.h"

#include "Engine/ResourceTestPick.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HotSpotImage
        : public HotSpot
    {
        DECLARE_FACTORABLE( HotSpotImage );
        DECLARE_VISITABLE( HotSpot );

    public:
        HotSpotImage();
        ~HotSpotImage() override;

    public:
        void setResourceTestPick( const ResourceTestPickPtr & _resourceTestPick );
        const ResourceTestPickPtr & getResourceTestPick() const;

    public:
        void setAlphaTest( float _value );
        float getAlphaTest() const;

        uint32_t getWidth() const;
        uint32_t getHeight() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void _dispose() override;

    protected:
        bool testPoint( const RenderContext * _context, const mt::vec2f & _point ) const override;
        bool testRadius( const RenderContext * _context, const mt::vec2f & _point, float _radius ) const override;
        bool testPolygon( const RenderContext * _context, const mt::vec2f & _point, const Polygon & _polygon ) const override;

    protected:
        bool testBounds( const RenderContext * _context, float _left, float _right, float _top, float _bottom ) const override;

    public:
        void getWorldBoundingBox( mt::box2f * const _bb ) const;
        void getScreenBoundingBox( const RenderContext * _context, mt::box2f * const _bb ) const;

    protected:
        ResourceTestPickPtr m_resourceTestPick;

        float m_alphaTest;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotImage> HotSpotImagePtr;
    //////////////////////////////////////////////////////////////////////////
}
