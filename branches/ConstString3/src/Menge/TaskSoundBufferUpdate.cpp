/*
 *	TaskSoundBufferUpdate.cpp
 *
 *	Created by _Berserk_ on 5.5.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "TaskSoundBufferUpdate.h"
#	include "SoundEngine.h"
#	include "ThreadEngine.h"

#	include "LogEngine.h"

#	include <sstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TaskSoundBufferUpdate::TaskSoundBufferUpdate( SoundSourceDesc* _soundSource )
		: m_running(true)
		, m_soundSource(_soundSource)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TaskSoundBufferUpdate::~TaskSoundBufferUpdate()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void TaskSoundBufferUpdate::stop()
	{
		m_running = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TaskSoundBufferUpdate::_onMain()
 	{
		SoundBufferInterface* soundBuffer = m_soundSource->soundSourceInterface->getSoundBuffer();

		while( m_running == true )
		{
			soundBuffer->update();

			ThreadEngine::get()
				->sleep( 50 );
		}

		if( m_soundSource->state == Stopping )
		{
			m_soundSource->soundSourceInterface->stop();
		}
		else if( m_soundSource->state == Pausing )
		{
			m_soundSource->soundSourceInterface->pause();
		}

		//m_soundSource->taskSoundBufferUpdate = NULL;
		m_complete = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskSoundBufferUpdate::_onComplete()
	{
		if( m_soundSource->state == Stopping )
		{
			m_soundSource->state = Stopped;
		}
		else if( m_soundSource->state == Pausing )
		{
			m_soundSource->state = Paused;
		}
	//	else if( m_soundSource.state == NeedRestart )
	//	{
	//		m_soundSource.soundSourceInterface->stop();
	//	}
	}


	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
