#include "MockupRenderImageLocked.h"

#include "Interface/MemoryServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderImageLocked::MockupRenderImageLocked()
        : m_lockedPitch( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderImageLocked::~MockupRenderImageLocked()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImageLocked::initialize( const Rect & _lockedRect, uint32_t _hwChannels, uint32_t _hwDepth )
    {
        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        size_t size = (size_t)(_lockedRect.right - _lockedRect.left) * (_lockedRect.bottom - _lockedRect.top) * _hwChannels * _hwDepth;

        memory->newBuffer( size );

        m_lockedMemory = memory;
        m_lockedPitch = (size_t)(_lockedRect.right - _lockedRect.left) * _hwChannels * _hwDepth;
    }
    //////////////////////////////////////////////////////////////////////////
    Pointer MockupRenderImageLocked::getBuffer( size_t * const _pitch ) const
    {
        *_pitch = m_lockedPitch;

        void * buffer = m_lockedMemory->getBuffer();

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
}