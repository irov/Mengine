#	pragma once

#	include "HotSpotPolygon.h"

#   include "ResourceShape.h"

namespace Menge
{
    class HotSpotShape
        : public HotSpotPolygon
    {
    public:
        HotSpotShape();
        ~HotSpotShape();

    public:
        void setResourceShape( ResourceShape * _resourceShape );
        ResourceShape * getResourceShape() const;

    protected:
        bool _compile() override;
        void _release() override;

    private:
        ResourceHolder<ResourceShape> m_resourceShape;
    };
}	// namespace Menge
