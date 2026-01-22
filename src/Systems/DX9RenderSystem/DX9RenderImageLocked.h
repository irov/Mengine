#pragma once

#include "Interface/RenderImageLockedInterface.h"

#include "Environment/DirectX9/DX9RenderIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderImageLocked
        : public RenderImageLockedInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX9RenderImageLocked );

    public:
        DX9RenderImageLocked();
        ~DX9RenderImageLocked() override;

    public:
        void initialize( const Rect & _rect, const D3DLOCKED_RECT & _lockedRect );

    protected:
        const Rect & getLockedRect() const override;

    protected:
        Pointer getLockedBuffer( size_t * const _pitch ) const override;

    protected:
        Rect m_rect;
        D3DLOCKED_RECT m_lockedRect;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderImageLocked, RenderImageLockedInterface> DX9RenderImageLockedPtr;
    //////////////////////////////////////////////////////////////////////////
}