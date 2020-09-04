#pragma once

#include "HotSpotPolygon.h"

#include "ResourceShape.h"

namespace Mengine
{
    class HotSpotResourceShape
        : public HotSpotPolygon
    {
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

    private:
        ResourceShapePtr m_resourceShape;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<HotSpotResourceShape> HotSpotResourceShapePtr;
    //////////////////////////////////////////////////////////////////////////
}
