#include "interfaces.h"

#include "SoundBuffer.h"

class OpenALSoundSource : public SoundSourceInterface  
{
public:
	OpenALSoundSource(SoundNodeListenerInterface* _plr, SoundBufferInterface* _data, bool _streaming);
	~OpenALSoundSource();
	void			play(); 
	void			pause();
	void			stop();
	void			setLooped(bool _flag);
	void			setVolume(float _value);
	void			setPosition(const float* _position);
	void			setDistance(float _dist);
	void			setHeadMode(bool _flag);
	const float*	getPosition() const;
	float			getDistance() const;
	float			getVolume() const;
	bool			getHeadMode() const;
	bool			getLooped() const;
	bool			update();
private:
	bool			mLooping;
	bool			mHeadMode;
	bool			mIsStream;

	float			mVolume;
	float			mPosition[3];
	float			mMaxDistance;
	int				mCurrentBuffer;

	ALuint			mBuffer[2];			
	ALuint			mSource;
	ALuint			mFormat;

	SoundNodeListenerInterface*		mPlayer;
	SoundBufferInterface*			mBufferWithData;
	unsigned char*					mDataSoundBuffer;

	static const int BUFFER_SIZE = 65536;
};