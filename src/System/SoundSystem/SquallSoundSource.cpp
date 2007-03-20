#	include "SquallSoundSource.h"
#	include <assert.h>

#	include "squall.h"

int PauseCallback(int ChannelID, bool pause,void* UserData)
{
	SoundNodeListenerInterface* Listener = static_cast<SoundNodeListenerInterface*>(UserData);
	Listener->listenPaused();
	return 1;
}

int StopCallback(int ChannelID,void* UserData)
{
	SoundNodeListenerInterface* Listener = static_cast<SoundNodeListenerInterface*>(UserData);
	Listener->listenStopped();
	return 1;
}

SQUALLSoundSource::SQUALLSoundSource(int _Sample, SoundNodeListenerInterface* _listener)
	: SampleID(_Sample)
	, ChannelID(-1)
	, Listener(_listener)
	, InitCallback(false)
{
	if(Listener != NULL) InitCallback = true;
}

SQUALLSoundSource::~SQUALLSoundSource()
{
	SQUALL_Sample_Stop(SampleID);
	SQUALL_Sample_Unload(SampleID);
	SQUALL_Sample_Stop(ChannelID);
};

void SQUALLSoundSource::play()
{
	if (InitCallback == true)
	{
		ChannelID = SQUALL_Sample_Play(SampleID, 0, 0, 1);
		SQUALL_Channel_SetPauseWorker(ChannelID,PauseCallback,Listener);
		SQUALL_Channel_SetEndWorker(ChannelID,StopCallback,Listener);
		InitCallback = false;
	}
	else
	{
		SQUALL_Sample_Pause(SampleID, false);
	}
}

void SQUALLSoundSource::pause()
{
	SQUALL_Sample_Pause(SampleID, true);
}

void SQUALLSoundSource::stop()
{
	SQUALL_Sample_Stop(SampleID);
}

bool SQUALLSoundSource::isPlaying() const
{
	return SQUALL_Channel_Status(ChannelID) == SQUALL_CHANNEL_STATUS_PLAY;
}

void SQUALLSoundSource::setVolume(float vol)
{
	SQUALL_Channel_SetVolume(ChannelID,vol);
}

float SQUALLSoundSource::getVolume() const	
{
	return	SQUALL_Channel_GetVolume(ChannelID)/100.0f;
}

void SQUALLSoundSource::setPosition(float x, float y, float z)
{
	float pos[3] = {x,y,z};
	SQUALL_Channel_Set3DPosition(ChannelID,pos);
}

const float* SQUALLSoundSource::getPosition() const
{
	//SQUALL_Channel_Get3DPosition(channel,Position);
	return 0;
}

void SQUALLSoundSource::setLoop(bool loop)
{
	SQUALL_Channel_SetLoop(ChannelID,loop);
}

bool SQUALLSoundSource::isLooping() const
{
	return SQUALL_Channel_GetLoop(ChannelID);
}

double	SQUALLSoundSource::getLengthS()
{
	return SQUALL_Channel_GetLengthMs(ChannelID) / 1000.0;
}