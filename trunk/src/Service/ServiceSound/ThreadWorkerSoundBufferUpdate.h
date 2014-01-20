#	pragma once

#   include "Interface/SoundSystemInterface.h"

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
		bool onWork( size_t _id ) override;
		void onDone( size_t _id ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		SoundBufferInterface * m_soundBuffer;
	};

	typedef stdex::intrusive_ptr<ThreadWorkerSoundBufferUpdate> ThreadWorkerSoundBufferUpdatePtr;
}	// namespace Menge
