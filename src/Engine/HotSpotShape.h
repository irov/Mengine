#pragma once

#include "HotSpotPolygon.h"

#include "ResourceShape.h"

#include "Kernel/ResourceHolder.h"

namespace Mengine
{
    class HotSpotShape
        : public HotSpotPolygon
    {
    public:
        HotSpotShape();
        ~HotSpotShape() override;

    public:
		void setResourceShape( const ResourceShapePtr & _resourceShape );
		const ResourceShapePtr & getResourceShape() const;

    protected:
        bool _compile() override;
        void _release() override;

    private:
        ResourceHolder<ResourceShape> m_resourceShape;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<HotSpotShape> HotSpotShapePtr;
    //////////////////////////////////////////////////////////////////////////
}	
