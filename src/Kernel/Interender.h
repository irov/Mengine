#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/DummyPicker.h"

namespace Mengine
{
    class Interender
        : public Node
        , public BaseRender
        , public DummyPicker
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_PICKERABLE();

    public:
        Interender();
        ~Interender() override;

    protected:
        void render( const RenderContext * _context ) const override;

    protected:
        const RenderViewportInterfacePtr & getPickerViewport() const override;
        const RenderCameraInterfacePtr & getPickerCamera() const override;
    };
}
