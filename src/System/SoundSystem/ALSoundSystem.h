
#pragma once

#include "SoundSystemInterface.h"

#define MAX_SOURCE_NUM 16

enum DistanceModel
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
	float	getSoundVelocity()								{ return mSoundVelocity; }
	void	setDopplerFactor( float _factor );
	float	getDopplerFactor()								{ return mDopplerFactor; }
	void	setDistanceModel(DistanceModel _model);
	DistanceModel getDistanceModel()						{ return mDistanceModel; }

	ALuint	getFreeSource();

private:
	ALuint	m_sources[MAX_SOURCE_NUM];
	UINT	m_sourceNum;
	float mSoundVelocity;
	float mDopplerFactor;
	DistanceModel mDistanceModel;
	float mListenerPosition[3];
	float mListenerOrientation[6];
	ALCdevice* mDevice;
	ALCcontext_struct* mContext;

};