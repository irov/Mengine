#	pragma once

#	include "Interface\SoundSystemInterface.h"

class SQUALLSample : public SoundBufferInterface
{
public:
	SQUALLSample();
	int		SampleID;
	void	loadFromFile(const char* _filename, bool _isStream);
	void	loadFromMemory(void* _buffer, int _size, bool _newmem);
	void	unload();
};