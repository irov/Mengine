#	pragma once

#	include "Kernel/ThreadJob.h"

namespace Menge
{
    class ServiceProviderInterface;

	struct SoundSourceDesc;

	class ThreadWorkerSoundBufferUpdate
		: public ThreadWorkerInterface
	{
	public:
		ThreadWorkerSoundBufferUpdate();
		~ThreadWorkerSoundBufferUpdate();

    public:        
        void initialize( ServiceProviderInterface * _serviceProvider, SoundBufferInterface * _soundSource );

	public:
		bool onWork() override;
		void onDone() override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		SoundBufferInterface * m_soundBuffer;
	};
}	// namespace Menge
