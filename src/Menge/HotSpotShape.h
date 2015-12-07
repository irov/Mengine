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
		void setResourceShape( const ResourceShapePtr & _resourceShape );
		const ResourceShapePtr & getResourceShape() const;

    protected:
        bool _compile() override;
        void _release() override;

    private:
        ResourceHolder<ResourceShape> m_resourceShape;
    };
}	// namespace Menge
