#	pragma once

#	include "ALSoundSystem.h"

#	include "Vorbis/vorbisfile.h"

#	include <time.h>

class OggStreamSound : public SoundSourceInterface
{
public:
	OggStreamSound(void* _data, int _size, SoundNodeListenerInterface* _listener = NULL);
	~OggStreamSound();

	bool play();
	bool pause();
	bool stop();

	bool isPlaying() const;
	bool isPaused() const;
	bool isStopped() const;
	bool isInitial() const;

	void setPitch(float pitch);
	float getPitch() const;

	void setGain(float gain);
	float getGain() const;

	void setRelativeToListener(bool relative);
	bool isRelativeToListener() const;

	void setPosition(float x, float y, float z);
	const float* getPosition() const;

	void setDirection(float x, float y, float z);
	const float* getDirection() const;

	void setLoop(bool loop);
	bool isLooping() const;

	bool process();

	double	getSizeSec();
private:
	clock_t m_startTime;

	SoundNodeListenerInterface* m_listener;

	ALSoundInfo	m_info;

	ALsizei m_streampos;
	ALuint m_source;

	float m_pitch;
	float m_gain;
	float m_maxGain;
	float m_minGain;
	float m_maxDistance;
	float m_rolloffFactor;
	float m_refDistance;
	float m_outerConeGain;
	float m_innerConeAngle;
	float m_outerConeAngle;

	float m_pos[3];
	float m_vel[3];
	float m_dir[3];

	ALboolean m_sourceRelative;
	ALboolean m_loop;

	ALuint m_buffers[2];

	OggVorbis_File  m_oggStream;     
	vorbis_info*    m_vorbisInfo;    
	vorbis_comment* m_vorbisComment; 

	static size_t oggread(void * _ptr, size_t _size, size_t _nmemb, void *_datasource);
	static int    oggseek(void * _datasource, ogg_int64_t _offset, int _whence);
	static long   oggtell(void * _datasource);
	static int    oggclose(void * _datasource);
	bool	oggstream(ALuint buffer);
};