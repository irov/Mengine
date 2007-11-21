
#pragma once

#include "SoundSystemInterface.h"

class ALSoundSystem;
class ALSoundBuffer;

class ALSoundSource : public SoundSourceInterface
{
public:
	ALSoundSource(ALSoundSystem* _soundSystem);
	virtual ~ALSoundSource();

public:
	virtual void play();
	virtual void pause();
	virtual void stop();
	virtual bool isPlaying() const;

public:
	virtual void setVolume( float _volume );
	virtual float getVolume() const;

public:
	virtual void setPosition( float _x, float _y, float _z );
	virtual const float * getPosition() const;

public:
	virtual void setLooped( bool _loop );
	virtual bool isLooping() const;

public:
	virtual int getLengthMs();
	virtual int getPosMs();

public:
	void setSoundBuffer( ALSoundBuffer* _soundBuffer );
	void setAmbient(bool _ambient);
	bool isAmbient();
	void setSoundNodeListener(SoundNodeListenerInterface* _listener)	{ mListener = _listener; }

private:
	ALuint mSourceName;
	ALSoundBuffer* mSoundBuffer;
	float mPosition[3];
	SoundNodeListenerInterface* mListener;
	ALSoundSystem* m_soundSystem;
};
