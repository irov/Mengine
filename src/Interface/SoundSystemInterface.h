#	pragma once

struct	SoundDataDesc
{
	enum SOUND_TYPE	 
	{
		OGG,
		WAV,
	}	
	type;
	
	void*		buffer;
	size_t		size;
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

class SoundBufferInterface
{
public:
	virtual void	loadFromFile(const char* _filename, bool _isStream) = 0;
	virtual void	loadFromMemory(void* _buffer, int _size, bool _newmem) = 0;
	virtual void	unload() = 0;
};

class SoundSystemInterface
{
public:
	virtual void				setListenerOrient(float* _position, float* _front, float* top) = 0;
	virtual SoundSourceInterface*		createSoundSource(bool _isHeadMode, SoundBufferInterface * _sample, SoundNodeListenerInterface*	_listener = 0) = 0;
	virtual SoundBufferInterface * createSoundBuffer() = 0;
	virtual void releaseSoundBuffer(SoundBufferInterface * _soundBuffer) = 0;
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
	virtual int	getLengthMS() = 0;
};

bool	initInterfaceSystem(SoundSystemInterface** );
void	releaseInterfaceSystem(SoundSystemInterface* );