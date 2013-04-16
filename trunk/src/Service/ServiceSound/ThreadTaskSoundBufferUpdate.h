#	pragma once

#	include "Kernel/ThreadTask.h"

namespace Menge
{
    class ServiceProviderInterface;

	struct SoundSourceDesc;

	class ThreadTaskSoundBufferUpdate
		: public ThreadTask
	{
	public:
		ThreadTaskSoundBufferUpdate( ServiceProviderInterface * _serviceProvider, SoundSourceDesc* _source );
		~ThreadTaskSoundBufferUpdate();

	public:
		void stop();

	public:
		bool _onMain() override;
		void _onComplete() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		SoundSourceDesc* m_soundSource;
		bool m_running;
	};
}	// namespace Menge
