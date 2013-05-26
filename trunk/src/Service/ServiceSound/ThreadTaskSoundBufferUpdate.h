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
		ThreadTaskSoundBufferUpdate();
		~ThreadTaskSoundBufferUpdate();

    public:        
        void initialize( ServiceProviderInterface * _serviceProvider, SoundSourceDesc * _soundSource );

    public:
        bool _destroy() override;

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
