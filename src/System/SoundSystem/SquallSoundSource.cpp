#	include "SquallSoundSource.h"
#	include <assert.h>
#	include "squall.h"
#	include "math.h"
#	include "windows.h"
#	include "stdio.h"

//////////////////////////////////////////////////////////////////////////
int PauseCallback(int ChannelID, bool pause,void* UserData)
{
	printf("PauseCallback\n");
	SoundNodeListenerInterface* Listener = static_cast<SoundNodeListenerInterface*>(UserData);
	Listener->listenPaused(pause);
	return 1;
}
//////////////////////////////////////////////////////////////////////////
int StopCallback(int ChannelID,void* UserData)
{
	printf("StopCallback\n");
	SoundNodeListenerInterface* Listener = static_cast<SoundNodeListenerInterface*>(UserData);
	Listener->listenStopped();
	return 1;
}
//////////////////////////////////////////////////////////////////////////
SQUALLSoundSource::SQUALLSoundSource(int _Head, SQUALLSample * sample, SoundNodeListenerInterface* _listener)
	: ChannelID(-1)
	, Listener(_listener)
	, Head(_Head)
	, m_sample(sample)
{}
//////////////////////////////////////////////////////////////////////////
SQUALLSoundSource::~SQUALLSoundSource()
{
	printf("stopped and destoyed \n");
	stop();
}
//////////////////////////////////////////////////////////////////////////
void SQUALLSoundSource::play()
{
	if(!isPlaying())
	{
		int sampleID = m_sample->getSampleID();
		if (Head)
		{
			float pos[3] = {};
			ChannelID = SQUALL_Sample_Play3D(sampleID, 0, 0, 1, pos, 0); 
		}
		else
		{
			ChannelID = SQUALL_Sample_Play(sampleID, 0, 0, 1);
		}

		if(ChannelID < 0)
		{
			assert(!"Bad Channel!");
		}

		if(Listener != NULL)
		{
			int err = SQUALL_Channel_SetPauseWorker(ChannelID,PauseCallback,Listener);
			if(err < 0)
			{
				assert(!"Bad SQUALL_Channel_SetPauseWorker!");
			}

			err = SQUALL_Channel_SetEndWorker(ChannelID,StopCallback,Listener);
			if(err < 0)
			{
				assert(!"Bad SQUALL_Channel_SetEndWorker!");
			}
		}
	}
	else
	{
		if( SQUALL_CHANNEL_STATUS_PAUSE == SQUALL_Channel_Status(ChannelID) )
		{
			int err = SQUALL_Channel_Pause(ChannelID, false);
			if(err < 0)
			{
				assert(!"Bad SQUALL_Channel_Pause!");
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void SQUALLSoundSource::pause()
{
	int err = SQUALL_Channel_Pause(ChannelID, true);
	if(err < 0)
	{
		assert(!"Bad SQUALL_Channel_Pause!");
	}
}
//////////////////////////////////////////////////////////////////////////
void SQUALLSoundSource::stop()
{
	if(isPlaying()) 
	{
		int err = SQUALL_Channel_Stop(ChannelID);
		if(err < 0)
		{
			assert(!"Bad SQUALL_Channel_Stop!");
		}
	}
}
//////////////////////////////////////////////////////////////////////////
bool SQUALLSoundSource::isPlaying() const
{
	int status =  SQUALL_Channel_Status(ChannelID);
	return status == SQUALL_CHANNEL_STATUS_PLAY;
}
//////////////////////////////////////////////////////////////////////////
void SQUALLSoundSource::setVolume(float vol)
{
	float dxvol = log10( vol * 9.0f + 2.0f );

	if ( dxvol >= 1 ) dxvol = 1.0f;
	if ( dxvol <= 0 ) dxvol = 0.0f;

	int percent = int(dxvol * 100);
	int err = SQUALL_Channel_SetVolume(ChannelID,percent);

	if(err < 0)
	{
		assert(!"Bad SQUALL_Channel_SetVolume!");
	}
}
//////////////////////////////////////////////////////////////////////////
float SQUALLSoundSource::getVolume() const	
{
	int temp = SQUALL_Channel_GetVolume(ChannelID);
	if(temp < 0)
	{
		assert(!"Bad SQUALL_Channel_GetVolume!");
	}

	float v = (temp > 0) ? float(temp)/100.f : 0;

	return	v;
}
//////////////////////////////////////////////////////////////////////////
void SQUALLSoundSource::setPosition(float x, float y, float z)
{
	float pos[3] = {x,y,z};
	SQUALL_Channel_Set3DPosition(ChannelID,pos);
}
//////////////////////////////////////////////////////////////////////////
const float* SQUALLSoundSource::getPosition() const
{
	//SQUALL_Channel_Get3DPosition(ChannelID,Position);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void SQUALLSoundSource::setLooped(bool loop)
{
	SQUALL_Channel_SetLoop(ChannelID,loop);
}
//////////////////////////////////////////////////////////////////////////
bool SQUALLSoundSource::isLooping() const
{
	return SQUALL_Channel_GetLoop(ChannelID) == 1;
}
//////////////////////////////////////////////////////////////////////////
int	SQUALLSoundSource::getLengthMS()
{
	int val = SQUALL_Channel_GetLengthMs(ChannelID);
	if(val < 0)
	{
		assert(!"error on SQUALL_Channel_GetLengthMs");
	}
	return val;
}
//////////////////////////////////////////////////////////////////////////
int SQUALLSoundSource::getPosMs()
{
	int val = SQUALL_Channel_GetPlayPositionMs(ChannelID);
	if(val < 0)
	{
		assert(!"error on SQUALL_Channel_GetPlayPositionMs");
	}
	return val;
}
//////////////////////////////////////////////////////////////////////////