
#pragma once

#include "../../Interface/SoundSystemInterface.h"


//class ALSoundSystem;
#include "ALSoundSystem.h"

class ALSoundBuffer;

class ALSoundSource : public SoundSourceInterface
{
public:
	ALSoundSource(ALSoundSystem* _soundSystem);
	~ALSoundSource();

public:
	virtual void play();
	virtual void pause();
	virtual void stop();
	virtual bool isPlaying() const			{ return m_playing; }

public:
	virtual void setVolume( float _volume );
	virtual float getVolume() const			{ return m_volume; }

public:
	virtual void setPosition( float _x, float _y, float _z );
	virtual const float * getPosition() const;

public:
	virtual void setLooped( bool _loop )	{ m_looped = _loop; }
	virtual bool isLooping() const			{ return m_looped; }

public:
	virtual int getLengthMs();
	virtual int getPosMs();
	
	virtual void setSoundNodeListener(SoundNodeListenerInterface* _listener)	{ m_listener = _listener; }

public:
	void setSoundBuffer( ALSoundBuffer* _soundBuffer );
	void setAmbient(bool _ambient)		 { m_ambient = _ambient; }
	bool isAmbient()	{ return m_ambient; }
	bool isBusy()	{ return m_busy; }
	void setUsed(bool _use)	{ m_busy = _use; }
	SoundNodeListenerInterface* getListener()	{ return m_listener; }

private:
	TALSourceName* m_sourceName;
	ALSoundBuffer* m_soundBuffer;
	ALSoundSystem* m_soundSystem;
	bool m_busy;
	bool m_ambient;
	bool m_playing;
	bool m_looped;
	float m_position[3];
	float m_volume;
	SoundNodeListenerInterface* m_listener;
	HANDLE m_stopCallbackHandle;

	void _updateParams();
};
