
#pragma once

#include "SoundSystemInterface.h"

#define MAX_SOURCE_NUM 16

enum EDistanceModel
{
	None = 0,
	InverseDistance,
	InverseDistanceClamped
};

class ALSoundSystem : public SoundSystemInterface
{
public:
	ALSoundSystem();
	virtual ~ALSoundSystem();

public:
	virtual void    setListenerOrient( float * _position, float * _front, float * top);
	virtual SoundSourceInterface*   createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample, SoundNodeListenerInterface * _listener );

	virtual SoundBufferInterface *  createSoundBufferFromFile( const char * _filename, bool _isStream );
	virtual SoundBufferInterface *  createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem );

	virtual void    releaseSoundBuffer( SoundBufferInterface * _soundBuffer );
	virtual void    releaseSoundNode( SoundSourceInterface * _sn );

public:
	void	setSoundVelocity( float _velocity );
	float	getSoundVelocity()								{ return m_soundVelocity; }
	void	setDopplerFactor( float _factor );
	float	getDopplerFactor()								{ return m_dopplerFactor; }
	void	setDistanceModel(EDistanceModel _model);
	EDistanceModel getDistanceModel()						{ return m_distanceModel; }

	ALuint	getFreeSource();

private:
	ALuint	m_sources[MAX_SOURCE_NUM];
	int m_sourceNum;
	float m_soundVelocity;
	float m_dopplerFactor;
	EDistanceModel m_distanceModel;
	float m_listenerPosition[3];
	float m_listenerOrientation[6];
	ALCdevice* m_device;
	ALCcontext_struct* m_context;

};