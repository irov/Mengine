#	include "SoundBuffer.h"
#	include "windows.h"
#	include <assert.h>
#	include "squall.h"
#	include "math.h"

//////////////////////////////////////////////////////////////////////////
SQUALLSample::SQUALLSample()
: SampleID(-1)
{

}
//////////////////////////////////////////////////////////////////////////
bool SQUALLSample::loadFromFile(const char* _filename, bool _isStream)
{
	SampleID = SQUALL_Sample_LoadFile((char*)_filename, !_isStream, 0);
	if(SampleID < 0)
	{	
		//assert(!"Error with SQUALL_Sample_LoadFile");
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool SQUALLSample::loadFromMemory(void* _buffer, int _size, bool _newmem)
{
	SampleID = SQUALL_Sample_LoadFromMemory(_buffer, _size, _newmem, 0);
	if(SampleID < 0)
	{	
		//assert(!"Error with SQUALL_Sample_LoadFromMemory");
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void SQUALLSample::unload()
{
	SQUALL_Sample_Unload(SampleID);
}
//////////////////////////////////////////////////////////////////////////
int SQUALLSample::getSampleID() const
{
	return SampleID;
}