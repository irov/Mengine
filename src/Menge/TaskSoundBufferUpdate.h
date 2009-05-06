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
	struct TSoundSource;

	class TaskSoundBufferUpdate
		: public Task
	{
	public:
		TaskSoundBufferUpdate( TSoundSource* _source );

	public:
		void stop();

	public:
		void main() override;
		void postMain() override;

	protected:
		TSoundSource* m_soundSource;
		bool m_running;
		bool m_pause;
	};
}	// namespace Menge
