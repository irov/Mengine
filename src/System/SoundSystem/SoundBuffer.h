#	pragma once

#	include "Interface\SoundSystemInterface.h"

class SQUALLSample : public SoundBufferInterface
{
public:
	SQUALLSample();

public:
	bool loadFromFile(const char* _filename, bool _isStream);
	bool loadFromMemory(void* _buffer, int _size, bool _newmem);
	void unload();

public:
	int getSampleID() const;

protected:
	int		SampleID;
};
