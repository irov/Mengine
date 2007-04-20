#	pragma once

#	include "..\..\Menge\SoundSystemInterface.h"

class SQUALLSoundSource
	: public SoundSourceInterface
{
public:
	SQUALLSoundSource(int _Sample, int _Head, SoundNodeListenerInterface* _listener = 0);
	~SQUALLSoundSource();
	void play();
	void pause();
	void stop();
	bool isPlaying() const;
	void setVolume(float vol);
	float getVolume() const;
	void setPosition(float x, float y, float z);
	const float* getPosition() const;
	void setLoop(bool loop);
	bool isLooping() const;
	double	getLengthS();
private:
	int SampleID;
	int ChannelID;
	int Head;
	SoundNodeListenerInterface* Listener;
	bool InitCallback;
};