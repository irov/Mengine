#	pragma once

enum SOUND_TYPE	 
{
	OGG,
	WAV,
	MP3,
};

struct	SoundDataDesc
{
	SOUND_TYPE	type;
	void*		buffer;
	size_t		size;
	bool		isStreamSound;
};

class	SoundSourceInterface;

class	SoundNodeListenerInterface
{
public:
	virtual bool listenRecycled(SoundSourceInterface*	_sn) = 0;
	virtual void listenStoped(SoundSourceInterface*	_sn) = 0;
	virtual void listenEnded(SoundSourceInterface*	_sn) = 0;
};

class	SoundSourceInterface
{
public:
	SoundSourceInterface(){};
	virtual ~SoundSourceInterface(){};
	virtual	void			release() = 0;
	virtual void			play() = 0; 
	virtual void			pause() = 0;
	virtual void			stop() = 0;
	virtual bool			isPlaying() const = 0;
	virtual	bool			updateSoundBuffer() = 0;
	virtual void			setLooped(bool _flag) = 0;
	virtual bool			getLooped() const = 0;
	virtual void			setVolume(float _value) = 0;
	virtual float			getVolume() const = 0;
	virtual void			setPosition(const float* _position) = 0;
	virtual const float*	getPosition() const = 0;
	virtual void			setMaxDistance(float _dist) = 0;
	virtual float			getMaxDistance() const = 0;
	virtual void			setHeadMode(bool _flag) = 0;
	virtual bool			getHeadMode() const = 0;
	virtual double			getTotalSize()	const = 0;
	virtual	double			getPos()	const = 0;
};

class SoundSystemInterface
{
public:
	virtual void						setListenerOrient(const float* _position, const float* _updir) = 0;
	virtual SoundSourceInterface*		loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener = 0) = 0;
	virtual void						releaseSoundNode(SoundSourceInterface* _sn) = 0;
};

bool	initInterfaceSystem(SoundSystemInterface** );
void	releaseInterfaceSystem(SoundSystemInterface* );