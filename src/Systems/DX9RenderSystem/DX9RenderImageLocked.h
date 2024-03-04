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
        void initialize( const D3DLOCKED_RECT & _lockedRect );

    protected:
        Pointer getBuffer( size_t * const _pitch ) const override;

    protected:
        D3DLOCKED_RECT m_lockedRect;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderImageLocked, RenderImageLockedInterface> DX9RenderImageLockedPtr;
    //////////////////////////////////////////////////////////////////////////
}