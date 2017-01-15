#	include "ThreadWorkerSoundBufferUpdate.h"

#   include "Interface/ThreadSystemInterface.h"

#   include "SoundEngine.h"

#	include "Logger/Logger.h"

#	include <sstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadWorkerSoundBufferUpdate::ThreadWorkerSoundBufferUpdate()
		: m_done(false)
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
        (void)_id;

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
		(void)_id;

		m_done = true;        
	}
}	// namespace Menge
