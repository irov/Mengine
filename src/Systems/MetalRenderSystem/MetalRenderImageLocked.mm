#include "MetalRenderImageLocked.h"

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderImageLocked::MetalRenderImageLocked()
        : m_lockedPitch( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderImageLocked::~MetalRenderImageLocked() = default;
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderImageLocked::initialize( size_t _size, size_t _pitch, const Rect & _rect )
    {
        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        memory->newBuffer( _size );

        m_lockedMemory = memory;
        m_lockedRect = _rect;
        m_lockedPitch = _pitch;
    }
    //////////////////////////////////////////////////////////////////////////
    const Rect & MetalRenderImageLocked::getLockedRect() const
    {
        return m_lockedRect;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MetalRenderImageLocked::getBuffer( size_t * const _pitch ) const
    {
        *_pitch = m_lockedPitch;
        return m_lockedMemory->getBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
}

