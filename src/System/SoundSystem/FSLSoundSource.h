#	include "../../Menge/SoundSystemInterface.h"
#	include "freesl.h"

class FSLSound : public SoundSourceInterface
{
public:
	FSLSound(void* _data, int _size, SoundNodeListenerInterface* _listener, bool _isStreaming, const char* strFileFormat);
	~FSLSound();

	bool play();
	bool pause();
	bool stop();

	bool isPlaying() const;
	bool isPaused() const;
	bool isStopped() const;
	
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

	double	getSizeSec();
private:
	FSLsound obj;
	SoundNodeListenerInterface* m_listener;
};