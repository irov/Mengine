#pragma once

#include "Kernel/Resource.h"
#include "Kernel/Polygon.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceShape
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceShape );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceShape();
        ~ResourceShape() override;

    public:
        void setPolygon( const Polygon & _polygon );
        const Polygon & getPolygon() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        Polygon m_polygon;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceShape> ResourceShapePtr;
    //////////////////////////////////////////////////////////////////////////
}
