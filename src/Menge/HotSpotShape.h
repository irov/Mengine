#	pragma once

#	include "HotSpot.h"

namespace Menge
{
    class ResourceShape;

    class HotSpotShape
        : public HotSpot
    {
    public:
        HotSpotShape();
        ~HotSpotShape();

    public:
        bool testPolygon( const mt::mat4f& _transform, const Polygon& _screenPoly, const mt::mat4f& _screenTransform ) override;
        bool testRadius( const mt::mat4f& _transform, float _radius, const mt::mat4f& _screenTransform ) override;

    public:
        void setResourceShape( ResourceShape * _resourceShape );
        ResourceShape * getResourceShape() const;

    protected:
        bool _compile() override;
        void _release() override;

    private:
        ResourceShape * m_resourceShape;
    };
}	// namespace Menge
