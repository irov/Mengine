#include "MockupRenderImageLocked.h"

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/PixelFormatHelper.h"

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
    void MockupRenderImageLocked::initialize( const Rect & _lockedRect, EPixelFormat _hwPixelFormat )
    {
        MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        uint32_t hwChannels = Helper::getPixelFormatChannels( _hwPixelFormat );
        uint32_t hwDepth = Helper::getPixelFormatDepth( _hwPixelFormat );

        size_t pitch = (size_t)(_lockedRect.right - _lockedRect.left) * hwChannels * hwDepth;
        
        uint32_t height = _lockedRect.bottom - _lockedRect.top;
        size_t size = pitch * (size_t)height;

        memory->newBuffer( size );

        m_lockedMemory = memory;
        m_lockedPitch = pitch;
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