#pragma once

#include "Interface/RenderImageLockedInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Rect.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderImageLocked
        : public RenderImageLockedInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MetalRenderImageLocked );

    public:
        MetalRenderImageLocked();
        ~MetalRenderImageLocked();

    public:
        void initialize( size_t _size, size_t _pitch, const Rect & _rect );

    public:
        const Rect & getLockedRect() const override;

    public:
        Pointer getLockedBuffer( size_t * const _pitch ) const override;

    protected:
        MemoryInterfacePtr m_lockedMemory;
        Rect m_lockedRect;
        size_t m_lockedPitch;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderImageLocked, RenderImageLockedInterface> MetalRenderImageLockedPtr;
    //////////////////////////////////////////////////////////////////////////
}
