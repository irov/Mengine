#include "ThreadWorkerSoundBufferUpdate.h"

#include "SoundService.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadWorkerSoundBufferUpdate::ThreadWorkerSoundBufferUpdate()
        : m_done( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadWorkerSoundBufferUpdate::~ThreadWorkerSoundBufferUpdate()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadWorkerSoundBufferUpdate::initialize( const SoundBufferInterfacePtr & _soundBuffer )
    {
        m_soundBuffer = _soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadWorkerSoundBufferUpdate::isDone() const
    {
        return m_done;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadWorkerSoundBufferUpdate::onWork( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        bool continue_update = m_soundBuffer->update();

        if( continue_update == false )
        {
            m_done = true;
        }

        return continue_update;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadWorkerSoundBufferUpdate::onDone( uint32_t _id )
    {
        MENGINE_UNUSED( _id );

        m_done = true;
    }
}
