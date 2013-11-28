#	pragma once

#	include "HotSpot.h"

#   include "ResourceShape.h"

namespace Menge
{
    class HotSpotShape
        : public HotSpot
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
