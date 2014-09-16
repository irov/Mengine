#	pragma once

#   include "Interface/SoundSystemInterface.h"

#	include "Kernel/ThreadJob.h"

namespace Menge
{
	class ThreadWorkerSoundBufferUpdate
		: public ThreadWorkerInterface
	{
	public:
		ThreadWorkerSoundBufferUpdate();
		~ThreadWorkerSoundBufferUpdate();

    public:        
        void initialize( ServiceProviderInterface * _serviceProvider, const SoundBufferInterfacePtr & _soundSource );

	public:
		bool isDone() const;

	public:
		bool onWork( size_t _id ) override;
		void onDone( size_t _id ) override;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		SoundBufferInterfacePtr m_soundBuffer;

		volatile bool m_done;
	};

	typedef stdex::intrusive_ptr<ThreadWorkerSoundBufferUpdate> ThreadWorkerSoundBufferUpdatePtr;
}	// namespace Menge
