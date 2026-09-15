#pragma once

#include "Interface/RenderScissorInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Viewport.h"

#include "math/mat4.h"

#include "Mosaic/Types.hpp"

namespace Mengine
{
    class MosaicRenderScissor
        : public Factorable
        , public RenderScissorInterface
    {
        DECLARE_FACTORABLE( MosaicRenderScissor );

    public:
        MosaicRenderScissor();
        ~MosaicRenderScissor() override;

    public:
        void setScissorViewport( const mt::mat4f & _wm, const Mosaic::Rect & _rect, const RenderScissorInterface * _parent );

    public:
        const Viewport & getScissorViewportWM() const override;

    protected:
        Viewport m_viewportWM;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MosaicRenderScissor> MosaicRenderScissorPtr;
    //////////////////////////////////////////////////////////////////////////
}
