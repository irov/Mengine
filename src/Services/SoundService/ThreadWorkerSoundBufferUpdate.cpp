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
    void ThreadWorkerSoundBufferUpdate::onThreadWorkerUpdate( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadWorkerSoundBufferUpdate::onThreadWorkerWork( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        bool continue_update = m_soundBuffer->updateSoundBuffer();

        if( continue_update == false )
        {
            m_done = true;
        }

        return continue_update;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadWorkerSoundBufferUpdate::onThreadWorkerDone( UniqueId _id )
    {
        MENGINE_UNUSED( _id );

        m_done = true;
    }
    //////////////////////////////////////////////////////////////////////////
}
