#	pragma once

#	include "Kernel/ResourceReference.h"

#   include "Core/Polygon.h"

namespace Menge
{
    class ResourceShape
        : public ResourceReference
    {
        RESOURCE_DECLARE( ResourceShape );

    public:
        ResourceShape();

    public:
        const Polygon & getPolygon() const;

    public:
        bool _loader( const Metabuf::Metadata * _meta ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        bool _isValid() const override;
        
    protected:
        Polygon m_polygon;
    };
}	// namespace Menge
