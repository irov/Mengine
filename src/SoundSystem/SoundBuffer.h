#pragma once

#include <assert.h>
#include "openal/al.h"
#include "openal/alc.h"
#include "vorbis/vorbisfile.h"
#include <memory.h>

class	SoundBufferInterface
{
public:
	SoundBufferInterface(void* _buffer, size_t _size);
	virtual ~SoundBufferInterface();

	virtual	int		getNumChannels()	const;
	virtual	void	setNumChannels(int _rhs);

	virtual	int		getFrequency()		const;
	virtual	void	setFrequency(int _rhs);

	virtual	int		getBitsPerSample()	const;
	virtual	void	setBitsPerSample(int _rhs);

	virtual	int		getSize()	const;
	virtual	void	setSize(int _rhs);

	virtual	bool	atEnd()		const;
	virtual	int		getPos()	const;
	virtual	long	getLong();
	virtual	int		seekCur(int _delta);
	virtual	int		seekAbs(int _offs);
	virtual	int		getPtr(void* _ptr, int _len);
	virtual	int		getDataSoundSize() const = 0;
	virtual	int		read(unsigned char * _buffer, int _size = -1) = 0;
	virtual	bool	seek(float _time) = 0;
private:
	int				mNumChannels;
	int				mFreq;
	int				mBitsPerSample;
	int				mLength;
	int				mPos;
	unsigned char*	mBits;
};