#	include "ThreadTaskSoundBufferUpdate.h"

#   include "Interface/ThreadSystemInterface.h"

#   include "SoundEngine.h"

#	include "Logger/Logger.h"

#	include <sstream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadWorkerSoundBufferUpdate::ThreadWorkerSoundBufferUpdate()
		: m_serviceProvider(nullptr)
		, m_soundBuffer(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadWorkerSoundBufferUpdate::~ThreadWorkerSoundBufferUpdate()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ThreadWorkerSoundBufferUpdate::initialize( ServiceProviderInterface * _serviceProvider, SoundBufferInterface * _soundBuffer )
    {
        m_serviceProvider = _serviceProvider;
        m_soundBuffer = _soundBuffer;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ThreadWorkerSoundBufferUpdate::onWork()
 	{
        m_soundBuffer->update();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadWorkerSoundBufferUpdate::onDone()
	{
        this->destroy();
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
