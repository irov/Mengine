#	pragma once

#	include "OpenAL/altypes.h"

class	SoundBufferInterface
{
public:
	SoundBufferInterface(void* _buffer, size_t _size);
	virtual ~SoundBufferInterface();

public:
	virtual	size_t	getNumChannels()	const;
	virtual	void	setNumChannels(size_t _rhs);

	virtual	ALsizei	getFrequency()		const;
	virtual	void	setFrequency(ALsizei _rhs);

	virtual	int		getBitsPerSample()	const;
	virtual	void	setBitsPerSample(int _rhs);

	virtual size_t		getLength() const;
	virtual	void	setSize(size_t _rhs);

	virtual	bool	atEnd()		const;
	virtual	size_t	getPos()	const;
	virtual	long	getLong();
	virtual	size_t	seekCur(int _delta);
	virtual	size_t	seekAbs(size_t _offs);
	virtual	size_t	getPtr(void* _ptr, size_t _len);
	virtual	size_t	getDataSoundSize() const = 0;
	virtual	ALsizei	read(unsigned char * _buffer, size_t _size = -1) = 0;
	virtual	bool	seek(float _time) = 0;

	virtual double	getTotalTime() = 0;

private:
	size_t	m_numChannels;
	ALsizei m_freq;
	int		m_bitsPerSample;
	size_t	m_length;
	size_t	m_pos;
	unsigned char* m_bits;
};