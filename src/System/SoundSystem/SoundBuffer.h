#	pragma once

class	SoundBufferInterface
{
public:
	SoundBufferInterface(void* _buffer, size_t _size);
	virtual ~SoundBufferInterface();

public:
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

	virtual double	getTotalTime() = 0;

private:
	int m_numChannels;
	int m_freq;
	int m_bitsPerSample;
	int m_length;
	int m_pos;
	unsigned char* m_bits;
};