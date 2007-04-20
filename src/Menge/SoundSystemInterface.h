#pragma once
#include <string>

struct	SoundDataDesc
{
	enum SOUND_TYPE	 
	{
		OGG,
		WAV,
	}	type;
	
	/*void*		buffer;
	size_t		size;*/
	std::string  FILENAME_TEMP_FOR_TEST;
	bool		isRelativeToListener;
	bool		isStreamSound;
};

class	SoundSourceInterface;

class	SoundNodeListenerInterface
{
public:
	virtual void listenPaused(bool _pause) = 0;
	virtual void listenStopped() = 0;
};

class SoundSystemInterface
{
public:
	virtual void				setListenerOrient(float* _position, float* _updir) = 0;
	virtual SoundSourceInterface*		loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener = 0) = 0;
	virtual void				releaseSoundNode(SoundSourceInterface* _sn) = 0;
};

class SoundSourceInterface
{
public:
	virtual ~SoundSourceInterface(){};
	virtual void play() = 0;
	virtual void pause() = 0;
	virtual void stop() = 0;
	virtual bool isPlaying() const = 0;
	virtual void setVolume(float vol) = 0;
	virtual float getVolume() const = 0;
	virtual void setPosition(float x, float y, float z) = 0;
	virtual const float* getPosition() const = 0;
	virtual void setLoop(bool loop) = 0;
	virtual bool isLooping() const = 0;
	virtual double	getLengthS() = 0;
};

bool	initInterfaceSystem(SoundSystemInterface** );
void	releaseInterfaceSystem(SoundSystemInterface* );