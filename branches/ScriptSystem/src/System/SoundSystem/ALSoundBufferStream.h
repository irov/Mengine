
#pragma once

#	include "ALSoundBuffer.h"

#	include "Vorbis/Vorbisfile.h"
#	include <fstream>
//#	include "boost/thread.hpp"
namespace boost
{
	class thread;
}


//////////////////////////////////////////////////////////////////////////
/*class UpdateThread
{
public:
	UpdateThread( ALuint _alID1, ALuint _alID2, ALuint _source,
		ALenum _format, ALuint _frequency, ALuint _channels,
		OggVorbis_File* _oggFile, unsigned int _bufferSize );
	~UpdateThread();

	void operator()();
	void stop();

private:
	ALuint m_alID1, m_alID2;
	ALuint m_source;
	ALenum m_format;
	ALuint m_frequency;
	ALuint m_channels;
	OggVorbis_File* m_oggFile;
	unsigned int m_bufferSize;
	volatile bool m_playing;
	char* m_buffer;
};*/
//////////////////////////////////////////////////////////////////////////
class ALSoundBufferStream 
	: public ALSoundBuffer
{
public:
	ALSoundBufferStream();
	virtual ~ALSoundBufferStream();

	bool isStreamed() const;
	bool loadOgg( Menge::DataStreamInterface* _stream );

	void start( ALuint source );
	bool update();
	void stop();

	int getPosMs();
	void setPosMs( float _posMs );

	void threadLoop_();

protected:

	ALuint m_alID2;

	// OpenAL format of the sound data.
	ALenum m_format;
	// Frequency of the sound data.
	ALuint m_frequency;
	ALuint m_channels;
	//Source to update.
	ALuint m_source;

	char* m_buffer;
	Menge::DataStreamInterface* m_stream;
	OggVorbis_File m_oggFile;	// The file structure
	unsigned int m_bufferSize;	// Size of the buffer in bytes
	bool m_looping;				// Are we looping or not?
	bool m_updating;

	//UpdateThread* m_thread;
	boost::thread* m_boostThread;
};
//////////////////////////////////////////////////////////////////////////