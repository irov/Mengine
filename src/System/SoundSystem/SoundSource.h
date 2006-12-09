#	include "interfaces.h"

#	include "OpenAL/altypes.h"

#	include <time.h>

class SoundBufferInterface;

class OpenALSoundSource 
	: public SoundSourceInterface  
{
public:
	OpenALSoundSource(SoundNodeListenerInterface* _plr, SoundBufferInterface* _data, bool _streaming);
	~OpenALSoundSource();

public:
	void			play(); 
	void			pause();
	void			stop();
	void			setLooped(bool _flag);
	void			setVolume(float _value);
	void			setPosition(const float* _position);

	void			setMaxDistance(float _dist);
	float			getMaxDistance() const;
	bool			isPlaying() const;

	void			setHeadMode(bool _flag);
	const float*	getPosition() const;
	float			getVolume() const;
	bool			getHeadMode() const;
	bool			getLooped() const;
	bool			updateSoundBuffer();

	double			getTotalSize()	const;
	double			getPos() const;

	void			release();
private:

	bool m_looping;
	bool m_headMode;
	bool m_isStream;

	clock_t m_startTime;
	float m_vol;
	float m_pos[3];
	float m_maxDistance;
	int m_currentBuffer;

	ALuint m_buffer[2];			
	ALuint m_source;
	ALuint m_format;

	SoundNodeListenerInterface* m_listener;
	SoundBufferInterface* m_dataBuffer;
	unsigned char*  m_dataSoundBuffer;

	static const int BUFFER_SIZE = 65536;
};