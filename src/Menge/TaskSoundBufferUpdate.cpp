/*
 *	TaskSoundBufferUpdate.cpp
 *
 *	Created by _Berserk_ on 5.5.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "TaskSoundBufferUpdate.h"
#	include "SoundEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TaskSoundBufferUpdate::TaskSoundBufferUpdate( TSoundSource* _soundSource )
		: Task()
		, m_running( true )
		, m_soundSource( _soundSource )
		, m_pause( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskSoundBufferUpdate::stop()
	{
		m_running = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskSoundBufferUpdate::main()
	{
		SoundBufferInterface* soundBuffer = m_soundSource->soundSourceInterface->getSoundBuffer();
		while( m_running == true )
		{
			soundBuffer->update();
		}
		m_complete = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskSoundBufferUpdate::postMain()
	{
		if( m_soundSource->state == Stopping )
		{
			m_soundSource->soundSourceInterface->stop();
			m_soundSource->state = Stopped;
		}
		else if( m_soundSource->state == Pausing )
		{
			m_soundSource->soundSourceInterface->pause();
			m_soundSource->state = Paused;
		}
	//	else if( m_soundSource.state == NeedRestart )
	//	{
	//		m_soundSource.soundSourceInterface->stop();
	//	}
		m_soundSource->taskSoundBufferUpdate = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
