#	include "SquallSoundSource.h"
#	include "windows.h"
#	include <assert.h>
#	include "squall.h"
#	include "math.h"


int PauseCallback(int ChannelID, bool pause,void* UserData)
{
	printf("PauseCallback\n");
	SoundNodeListenerInterface* Listener = static_cast<SoundNodeListenerInterface*>(UserData);
	Listener->listenPaused(pause);
	return 1;
}

int StopCallback(int ChannelID,void* UserData)
{
	printf("StopCallback\n");
	SoundNodeListenerInterface* Listener = static_cast<SoundNodeListenerInterface*>(UserData);
	Listener->listenStopped();
	return 1;
}

SQUALLSoundSource::SQUALLSoundSource(int _Sample, int _Head, SoundNodeListenerInterface* _listener)
	: SampleID(_Sample)
	, ChannelID(-1)
	, Listener(_listener)
	, Head(_Head)
{
}

SQUALLSoundSource::~SQUALLSoundSource()
{
	stop();
	//SQUALL_Channel_Stop(ChannelID);
	//Sleep(200); //���� ?
};

void SQUALLSoundSource::play()
{
	if(!isPlaying())
	{
		if (Head)
		{
			float pos[3] = {};
			ChannelID = SQUALL_Sample_Play3D(SampleID, 0, 0, 1, pos, 0); 
		}
		else
		{
			ChannelID = SQUALL_Sample_Play(SampleID, 0, 0, 1);
		}

		SQUALL_Channel_SetPauseWorker(ChannelID,PauseCallback,Listener);
		SQUALL_Channel_SetEndWorker(ChannelID,StopCallback,Listener);
	}
	else
	{
		SQUALL_Channel_Pause(ChannelID, false);
	}
}

void SQUALLSoundSource::pause()
{
	SQUALL_Channel_Pause(ChannelID, true);
}

void SQUALLSoundSource::stop()
{
	if(isPlaying()) 
	{
		SQUALL_Channel_Stop(ChannelID);
	}
}

bool SQUALLSoundSource::isPlaying() const
{
	return SQUALL_Channel_Status(ChannelID) == SQUALL_CHANNEL_STATUS_PLAY;
}

void SQUALLSoundSource::setVolume(float vol)
{
	float dxvol = (20*100*(log10(vol / (double)128)));
	SQUALL_Channel_SetVolume(ChannelID,dxvol * 100);

	//SQUALL_Channel_SetVolume(ChannelID,vol * 100);
}

float SQUALLSoundSource::getVolume() const	
{
	float v = SQUALL_Channel_GetVolume(ChannelID)/100.f;
	return	v;
}

void SQUALLSoundSource::setPosition(float x, float y, float z)
{
	float pos[3] = {x,y,z};
	SQUALL_Channel_Set3DPosition(ChannelID,pos);
}

const float* SQUALLSoundSource::getPosition() const
{
	float Position[3]={};
	SQUALL_Channel_Get3DPosition(ChannelID,Position);
	return Position;
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