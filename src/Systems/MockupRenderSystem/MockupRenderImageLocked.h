#pragma once

#include "Interface/RenderImageLockedInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Rect.h"
#include "Kernel/Factorable.h"
#include "Kernel/PixelFormat.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderImageLocked
        : public RenderImageLockedInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MockupRenderImageLocked );

    public:
        MockupRenderImageLocked();
        ~MockupRenderImageLocked();

    public:
        void initialize( const Rect & _lockedRect, EPixelFormat _hwPixelFormat );

    protected:
        Pointer getBuffer( size_t * const _pitch ) const override;

    protected:
        MemoryInterfacePtr m_lockedMemory;

        size_t m_lockedPitch;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderImageLocked, RenderImageLockedInterface> MockupRenderImageLockedPtr;
    //////////////////////////////////////////////////////////////////////////
}