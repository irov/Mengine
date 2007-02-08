#	pragma once

#	include "ALSoundSystem.h"

class WavSound : public SoundSourceInterface
{
public:
	WavSound(void* _data, SoundNodeListenerInterface* _listener = 0);
	~WavSound();

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
	SoundNodeListenerInterface* m_listener;

	ALSoundInfo	m_info;

	ALuint m_source;
	ALuint m_buffer;

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
};