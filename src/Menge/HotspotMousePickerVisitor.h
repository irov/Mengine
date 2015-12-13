#   pragma once

#   include "Core/Visitor.h"

#	include "Math/vec2.h"

namespace Menge
{
    class Arrow;
    class HotSpot;
	class RenderViewportInterface;
    class RenderCameraInterface;

	class Layer;
	class Layer2D;
	class Layer2DIsometric;

    class HotspotMousePickerVisitor
        : public Visitor
        , public ConcreteVisitor<Layer2D>
		, public ConcreteVisitor<Layer2DIsometric>
    {
    public:
        HotspotMousePickerVisitor( HotSpot * _layerspaceHotspot, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const mt::vec2f & _point, Arrow * _arrow );

    public:
        bool test( Layer * _layer );

    protected:
        void accept( Layer2D * _visited ) override;
		void accept( Layer2DIsometric * _visited ) override;

	protected:
		void test_();

    protected:
        HotSpot * m_hotspot;
		const RenderViewportInterface * m_viewport;
        const RenderCameraInterface * m_camera;
        mt::vec2f m_point;
        Arrow * m_arrow;

        bool m_result;
    };
}