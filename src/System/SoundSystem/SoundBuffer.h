#	pragma once

class	SoundBufferInterface
{
public:
	SoundBufferInterface(void* _buffer, size_t _size);
	virtual ~SoundBufferInterface();

public:
	virtual	size_t getNumChannels() const;
	virtual	void	setNumChannels(size_t _rhs);

	virtual	size_t	getFrequency() const;
	virtual	void	setFrequency(size_t _rhs);

	virtual	size_t	getBitsPerSample()	const;
	virtual	void	setBitsPerSample(size_t _rhs);

	virtual	size_t	getSize() const;
	virtual	void	setSize(size_t _rhs);

	virtual	bool	atEnd() const;
	virtual	size_t	getPos() const;
	virtual	long	getLong();
	virtual	size_t	seekCur(size_t _delta);
	virtual	size_t	seekAbs(size_t _offs);
	virtual	size_t	getPtr(void* _ptr, size_t _len);
	virtual	size_t	getDataSoundSize() const = 0;
	virtual	size_t	read(unsigned char * _buffer, size_t _size = -1) = 0;
	virtual	bool	seek(float _time) = 0;

private:
	size_t			mNumChannels;
	size_t			mFreq;
	size_t			mBitsPerSample;
	size_t			mLength;
	size_t			mPos;
	unsigned char*	mBits;
};