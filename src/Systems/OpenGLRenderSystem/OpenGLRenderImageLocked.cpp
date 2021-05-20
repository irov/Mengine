#include "OpenGLRenderImageLocked.h"

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImageLocked::OpenGLRenderImageLocked()
        : m_lockedPitch( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenGLRenderImageLocked::~OpenGLRenderImageLocked()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderImageLocked::initialize( size_t _size, size_t _pitch, const Rect & _rect )
    {
        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory buffer (%zu) rect from %u %u to %u %u"
            , _size
            , _rect.left
            , _rect.top
            , _rect.right
            , _rect.bottom
        );

        void * buffer = memory->newBuffer( _size );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid new memory (%zu) rect from %u %u to %u %u"
            , _size
            , _rect.left
            , _rect.top
            , _rect.right
            , _rect.bottom
        );

        m_lockedMemory = memory;
        m_lockedRect = _rect;
        m_lockedPitch = _pitch;
    }
    //////////////////////////////////////////////////////////////////////////
    const Rect & OpenGLRenderImageLocked::getLockedRect() const
    {
        return m_lockedRect;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer OpenGLRenderImageLocked::getBuffer( size_t * const _pitch ) const
    {
        *_pitch = m_lockedPitch;

        void * buffer = m_lockedMemory->getBuffer();

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
}
