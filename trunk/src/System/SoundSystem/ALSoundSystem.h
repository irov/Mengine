
#pragma once

#include "../../Interface/SoundSystemInterface.h"

#include <vector>
#include <map>
#include <AL/AL.h>
#include <AL/ALC.h>

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

class ALSoundSystem 
	: public Menge::SoundSystemInterface
{
public:
	ALSoundSystem();
	virtual ~ALSoundSystem();
	
public:
	bool initialize( Menge::LogSystemInterface* _logSystem ) override;

	void setListenerOrient( float * _position, float * _front, float * top) override;
	Menge::SoundSourceInterface * createSoundSource( bool _isHeadMode, Menge::SoundBufferInterface * _sample, Menge::SoundNodeListenerInterface * _listener ) override;

	Menge::SoundBufferInterface* createSoundBuffer( Menge::DataStreamInterface* _stream, bool _isStream ) override;
	Menge::SoundBufferInterface * createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem ) override;

	void releaseSoundBuffer( Menge::SoundBufferInterface * _soundBuffer ) override;
	void releaseSoundNode( Menge::SoundSourceInterface * _sn ) override;

	bool setBlow( bool _active ) override;
	float getBlow() override;

	void setEnoughBlow( float _enoughBlow ) override;
	void setBlowCallback( Menge::SoundSulkCallbackInterface * _callback ) override;

	void update( float _timing ) override;

public:
	void	setSoundVelocity( float _velocity );
	float	getSoundVelocity();
	void	setDopplerFactor( float _factor );
	float	getDopplerFactor();
	void	setDistanceModel(EDistanceModel _model);
	EDistanceModel getDistanceModel();


	ALuint registerPlaying( ALSoundSource* _source, bool _stereo );
	void unregisterPlaying( ALSoundSource* _source, ALuint _alSource );
	void deletedSource( ALSoundSource* _source );

private:
	Menge::uint16 m_alSourcesNum;
	typedef std::map< ALuint, bool > TALSourceMap;
	TALSourceMap m_alSourcesStereo;
	TALSourceMap m_alSourcesMono;

	typedef std::map< ALSoundSource*, bool > TSourceMap;
	TSourceMap m_sources;

	typedef std::vector<ALSoundSource*> TSourceVector;
	TSourceVector m_playingSources;
	TSourceVector m_stoppingSources;
	TSourceVector m_addingSources;
	
	bool m_initialized;
	float m_soundVelocity;
	float m_dopplerFactor;
	EDistanceModel m_distanceModel;
	float m_listenerPosition[3];
	float m_listenerOrientation[6];
	ALCdevice* m_device;
	ALCcontext_struct* m_context;

	SulkSystem * m_sulk;

	Menge::LogSystemInterface* m_logSystem;
	ALuint getFreeSourceName_( bool stereo );
	void freeSource_( ALuint _source );

};