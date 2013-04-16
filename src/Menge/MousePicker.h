#   pragma once

#   include "Kernel/Visitor.h"

#   include "Layer2D.h"

namespace Menge
{
    class Arrow;
    class HotSpot;
    class RenderCameraInterface;

    class MousePicker
        : public Visitor
        , public ConcreteVisitor<Layer2D>
    {
    public:
        MousePicker( HotSpot * _layerspaceHotspot, RenderCameraInterface * _camera, const mt::vec2f & _point, Arrow * _arrow );

    public:
        bool test( Layer * _layer );

    protected:
        void accept( Layer2D * _visited ) override;

    protected:
        HotSpot * m_layerspaceHotspot;
        RenderCameraInterface * m_camera;
        mt::vec2f m_point;
        Arrow * m_arrow;

        bool m_result;
    };
}