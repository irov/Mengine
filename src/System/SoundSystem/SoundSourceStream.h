#	pragma once
#	include "interfaces.h"

#	include "SoundSource.h"
#	include "SoundBuffer.h"

#	include "OpenAL/altypes.h"

#	include <time.h>

class OpenALSoundSourceStream
	: public SoundSourceInterface
{
public:
	OpenALSoundSourceStream(SoundNodeListenerInterface* _plr, SoundBufferInterface* _data);
	~OpenALSoundSourceStream();
	void			play(); 
	void			pause();
	void			stop();

	void			setLooped(bool _flag);
	bool			getLooped() const;

	void			setVolume(float _value);
	float			getVolume() const;

	void			setPosition(const float* _position);
	const float*	getPosition() const;

	void			setMaxDistance(float _dist);
	float			getMaxDistance() const;

	void			setHeadMode(bool _flag);
	bool			getHeadMode() const;

	bool			isPlaying() const;

	bool			process();

	//return total size in sec.
	double			getTotalSize()	const;
	//return current slider pos in sec.
	double			getPos() const;
private:

	bool m_looping;
	bool m_headMode;

	clock_t m_startTime;
	float m_vol;
	float m_pos[3];
	float m_maxDistance;
	
	ALuint m_source;
	ALuint m_format;

	SoundNodeListenerInterface* m_listener;
	SoundBufferInterface* m_dataBuffer;

	static const size_t STREAMBUFFERSIZE = 1024 * 50;
	static const size_t STREAMFRAGMENTS = 5;
	static const size_t STREAMFRAGMENTSIZE 	= STREAMBUFFERSIZE / STREAMFRAGMENTS;

	bool _fillBufferAndQueue(ALuint buffer);
	ALuint buffers[STREAMFRAGMENTS];
};