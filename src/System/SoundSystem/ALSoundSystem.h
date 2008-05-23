
#pragma once

#include "../../Interface/SoundSystemInterface.h"

#include <vector>
#include <map>
#include <AL.h>
#include <ALC.h>

#define MAX_SOURCENAMES_NUM 100
#define MAX_SOUND_SOURCES 200

enum EDistanceModel
{
	None = 0,
	InverseDistance,
	InverseDistanceClamped
};

typedef struct
{
	ALuint name;
	bool   busy;
} 
TALSourceName;

class ALSoundSource;
class ALSoundBufferStream;
class SulkSystem;

class ALSoundSystem : public SoundSystemInterface
{
public:
	ALSoundSystem();
	virtual ~ALSoundSystem();

	bool initialize();

public:
	void setListenerOrient( float * _position, float * _front, float * top) override;
	SoundSourceInterface * createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample, SoundNodeListenerInterface * _listener ) override;

	SoundBufferInterface * createSoundBufferFromFile( const char * _filename, bool _isStream ) override;
	SoundBufferInterface * createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem ) override;

	void releaseSoundBuffer( SoundBufferInterface * _soundBuffer ) override;
	void releaseSoundNode( SoundSourceInterface * _sn ) override;

	bool setBlow( bool _active ) override;
	float getBlow() override;

	void setEnoughBlow( float _enoughBlow ) override;
	void setBlowCallback( SoundSulkCallbackInterface * _callback ) override;

	void update( float _timing ) override;


public:
	void	setSoundVelocity( float _velocity );
	float	getSoundVelocity()								{ return m_soundVelocity; }
	void	setDopplerFactor( float _factor );
	float	getDopplerFactor()								{ return m_dopplerFactor; }
	void	setDistanceModel(EDistanceModel _model);
	EDistanceModel getDistanceModel()						{ return m_distanceModel; }

	TALSourceName* getFreeSourceName();

	void	addStream( ALSoundBufferStream* _stream );
	void	removeStream( ALSoundBufferStream* _stream );
	void registerPlaying( ALSoundSource* _source, float _timeMs );
	void unregisterPlaying( ALSoundSource* _source );

private:
	TALSourceName m_sourceNames[MAX_SOURCENAMES_NUM];
	int m_sourceNamesNum;
	typedef std::vector<ALSoundSource*> TSourceVector;
	TSourceVector	m_sources;
	
	typedef std::vector<ALSoundBufferStream*> TVectorALSoundBufferStream;
	TVectorALSoundBufferStream m_streams;

	typedef std::map<ALSoundSource*, float> TSourcesMap;
	TSourcesMap m_playingSources;
	TSourceVector m_deletingSources;

	bool m_initialized;
	float m_soundVelocity;
	float m_dopplerFactor;
	EDistanceModel m_distanceModel;
	float m_listenerPosition[3];
	float m_listenerOrientation[6];
	ALCdevice* m_device;
	ALCcontext_struct* m_context;

	SulkSystem * m_sulk;
};