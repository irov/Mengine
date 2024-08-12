#pragma once

#include "Engine/HotSpotPolygon.h"

#include "Engine/ResourceShape.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class HotSpotResourceShape
        : public HotSpotPolygon
    {
        DECLARE_FACTORABLE( HotSpotResourceShape );
        DECLARE_VISITABLE( HotSpotPolygon );

    public:
        HotSpotResourceShape();
        ~HotSpotResourceShape() override;

    public:
        void setResourceShape( const ResourceShapePtr & _resourceShape );
        const ResourceShapePtr & getResourceShape() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void _dispose() override;

    protected:
        ResourceShapePtr m_resourceShape;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotResourceShape> HotSpotResourceShapePtr;
    //////////////////////////////////////////////////////////////////////////
}
