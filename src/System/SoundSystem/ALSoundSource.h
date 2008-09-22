
#pragma once

#include "../../Interface/SoundSystemInterface.h"

//#define _WIN32_WINNT 0x0500 
//#include <windows.h>

//class ALSoundSystem;
#include "ALSoundSystem.h"

class ALSoundBuffer;

class ALSoundSource 
	: public Menge::SoundSourceInterface
{
public:
	ALSoundSource( ALSoundSystem* _soundSystem );
	~ALSoundSource();

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
	
	virtual void setSoundNodeListener( Menge::SoundNodeListenerInterface* _listener );
	virtual void setSoundBuffer( Menge::SoundBufferInterface* _soundBuffer ) override;

public:
	void setAmbient( bool _ambient );
	bool isAmbient() const;
	Menge::SoundNodeListenerInterface* getListener();
	bool update( float _timing );
	void _onStop();

private:
	ALuint m_alSource;
	ALSoundBuffer* m_soundBuffer;
	ALSoundSystem* m_soundSystem;
	bool m_ambient;
	bool m_playing;
	bool m_looped;
	float m_position[3];
	float m_volume;
	Menge::SoundNodeListenerInterface* m_listener;

	void _updateParams();
	float m_counter;
};
