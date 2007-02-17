#pragma once

struct	SoundDataDesc
{
	enum SOUND_TYPE	 
	{
		OGG,
		WAV,
	}	type;
	void*		buffer;
	size_t		size;
	bool		isStreamSound;
};

class	SoundSourceInterface;

class	SoundNodeListenerInterface
{
public:
	virtual bool listenRecycled() = 0;
	virtual void listenPaused() = 0;
	virtual void listenStopped() = 0;
};

class SoundSystemInterface
{
public:
	virtual void				setListenerOrient(const float* _position, const float* _updir) = 0;
	virtual SoundSourceInterface*		loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener = 0) = 0;
	virtual void				releaseSoundNode(SoundSourceInterface* _sn) = 0;
};

bool	initInterfaceSystem(SoundSystemInterface** );
void	releaseInterfaceSystem(SoundSystemInterface* );

class SoundSourceInterface
{
public:
	virtual ~SoundSourceInterface(){};
	virtual bool play() = 0;
	virtual bool pause() = 0;
	virtual bool stop() = 0;

	virtual bool isPlaying() const = 0;
	virtual bool isPaused() const = 0;
	virtual bool isStopped() const = 0;
	
	virtual void setPitch(float pitch) = 0;
	virtual float getPitch() const = 0;

	virtual void setGain(float gain) = 0;
	virtual float getGain() const = 0;

	virtual void setRelativeToListener(bool relative) = 0;
	virtual bool isRelativeToListener() const = 0;

	virtual void setPosition(float x, float y, float z) = 0;
	virtual const float* getPosition() const = 0;

	virtual void setDirection(float x, float y, float z) = 0;
	virtual const float* getDirection() const = 0;

	virtual void setLoop(bool loop) = 0;
	virtual bool isLooping() const = 0;

	virtual double	getSizeSec() = 0;
};