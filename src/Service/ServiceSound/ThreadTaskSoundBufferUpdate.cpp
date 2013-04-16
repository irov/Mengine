#	include "ThreadTaskSoundBufferUpdate.h"

#   include "Interface/ThreadSystemInterface.h"

#   include "SoundEngine.h"

#	include "Logger/Logger.h"

#	include <sstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskSoundBufferUpdate::ThreadTaskSoundBufferUpdate( ServiceProviderInterface * _serviceProvider, SoundSourceDesc* _soundSource )
		: m_serviceProvider(_serviceProvider)
		, m_soundSource(_soundSource)
        , m_running(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskSoundBufferUpdate::~ThreadTaskSoundBufferUpdate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskSoundBufferUpdate::stop()
	{
		m_running = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskSoundBufferUpdate::_onMain()
 	{
		SoundBufferInterface* soundBuffer = m_soundSource->soundSourceInterface->getSoundBuffer();

		while( m_running == true )
		{
			soundBuffer->update();

            if( m_running == true )
            {
			    THREAD_SERVICE(m_serviceProvider)
				    ->sleep( 50 );
            }
		}

		if( m_soundSource->state == ESS_STOPPING )
		{
			m_soundSource->soundSourceInterface->stop();
		}
		else if( m_soundSource->state == ESS_PAUSING )
		{
			m_soundSource->soundSourceInterface->pause();
		}

		//m_soundSource->taskSoundBufferUpdate = NULL;
		m_complete = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskSoundBufferUpdate::_onComplete()
	{
		if( m_soundSource->state == ESS_STOPPING )
		{
			m_soundSource->state = ESS_STROPPED;
		}
		else if( m_soundSource->state == ESS_PAUSING )
		{
			m_soundSource->state = ESS_PAUSED;
		}
	//	else if( m_soundSource.state == NeedRestart )
	//	{
	//		m_soundSource.soundSourceInterface->stop();
	//	}
	}


	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
