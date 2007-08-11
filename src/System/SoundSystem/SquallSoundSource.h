#	pragma once

#	include "Interface\SoundSystemInterface.h"

#	include "SoundBuffer.h"

class SQUALLSoundSource
	: public SoundSourceInterface
{
public:
	SQUALLSoundSource(int _Head, SQUALLSample * sample, SoundNodeListenerInterface* _listener = 0);
	~SQUALLSoundSource();
	void play();
	void pause();
	void stop();
	bool isPlaying() const;
	void setVolume(float vol);
	float getVolume() const;
	void setPosition(float x, float y, float z);
	const float* getPosition() const;
	void setLooped(bool loop);
	bool isLooping() const;
	int	getLengthMS();
	int getPosMs();
private:
	SQUALLSample * m_sample;
	int ChannelID;
	int Head;
	SoundNodeListenerInterface* Listener;
	bool InitCallback;
};