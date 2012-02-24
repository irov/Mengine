/*
 *	TaskSoundBufferUpdate.h
 *
 *	Created by _Berserk_ on 5.5.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Task.h"

namespace Menge
{
	struct SoundSourceDesc;

	class TaskSoundBufferUpdate
		: public Task
	{
	public:
		TaskSoundBufferUpdate( SoundSourceDesc* _source );
		~TaskSoundBufferUpdate();

	public:
		void stop();

	public:
		bool _onMain() override;
		void _onComplete() override;

	protected:
		SoundSourceDesc* m_soundSource;
		bool m_running;
	};
}	// namespace Menge
