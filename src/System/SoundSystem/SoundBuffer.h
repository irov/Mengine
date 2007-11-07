#	pragma once

#	include "Interface\SoundSystemInterface.h"

class SQUALLSample : public SoundBufferInterface
{
public:
	SQUALLSample( int _id );

public:
	void unload();

public:
	int getSampleID() const;

protected:
	int		SampleID;
};
