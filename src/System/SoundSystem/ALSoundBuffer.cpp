
#	include "VorbisCallback.h"
#	include "ALSoundBuffer.h"
#	include "ALSoundSource.h"

#	include <algorithm>

#	define BUFFER_SIZE (4096)

//////////////////////////////////////////////////////////////////////////
ALSoundBuffer::ALSoundBuffer() 
: m_alID( 0 )
, m_lenghtMs( 0 )
, m_isEmpty( true )
, m_isStereo( false )
{
}
//////////////////////////////////////////////////////////////////////////
ALSoundBuffer::~ALSoundBuffer()
{
	for(unsigned int i = 0; i < m_sources.size(); i++)
	{
		m_sources[i]->stop();
		static_cast<ALSoundSource*>(m_sources[i])->setSoundBuffer(NULL);
	}
	alDeleteBuffers( 1, &m_alID );
}
//////////////////////////////////////////////////////////////////////////
unsigned int ALSoundBuffer::getLenghtMs() const
{
	return m_lenghtMs;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundBuffer::addSource(SoundSourceInterface *_source)
{
	m_sources.push_back(_source);
}
//////////////////////////////////////////////////////////////////////////
void ALSoundBuffer::removeSource(SoundSourceInterface *_source)
{
	TVectorSoundSourceInterface::iterator it = std::find(m_sources.begin(), m_sources.end(), _source);
	if( it != m_sources.end() )
	{
		m_sources.erase(it);
	}
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundBuffer::loadOgg( const char* _filename )
{
	if ( !m_isEmpty )
	{
		return false;
	}

	alGenBuffers( 1, &m_alID );

	int error;

	if ( (error = alGetError()) != AL_NO_ERROR )
	{
		return false;
	}

	// callbacks used to read the file
	ov_callbacks vorbisCallbacks;

	// Fill vorbisCallbacks struct
	vorbisCallbacks.read_func  = s_readOgg;
	vorbisCallbacks.seek_func  = s_seekOgg;
	vorbisCallbacks.tell_func  = s_tellOgg;
	vorbisCallbacks.close_func = s_closeOgg;

	std::ifstream oggFile;

	// Open Ogg file
	oggFile.open( _filename, std::ios_base::in | std::ios_base::binary );

	OggVorbis_File oggStream;

	// Open the file from memory.  We need to pass it a pointer to our data (in this case our SOggFile structure),
	// a pointer to our ogg stream (which the vorbis libs will fill up for us), and our callbacks
	if ( ov_open_callbacks( &oggFile, &oggStream, NULL, 0, vorbisCallbacks ) < 0 )
		return false;

	// take 4k out of the data segment, not the stack
	char pcmout[BUFFER_SIZE];
	int seek = 0;

	ogg_int64_t pcmlength = ov_pcm_total( &oggStream, -1 );
	m_lenghtMs = ov_time_total(&oggStream, -1) * 1000;
	vorbis_info *vi = ov_info( &oggStream, -1 );

	unsigned int memSize = (unsigned int)pcmlength * 2 * vi->channels;
	char *memory = new char[memSize];

	int eof = 0;
	int current_section;

	seek = decodeOggVorbis_( &oggStream, memory, memSize );

	// Формат данных в буфере OGG
	ALenum format;

	if ( vi->channels == 1 )
	{
		format = AL_FORMAT_MONO16;
	}
	else
	{
		format = AL_FORMAT_STEREO16;
		m_isStereo = true;
	}

	// Указатель на массив данных звука
	ALvoid *data = memory;

	// Размер этого массива
	ALsizei size = seek;

	// Частота звука в герцах
	ALsizei freq = vi->rate;

	alBufferData( m_alID, format, data, size, freq );

	if( alGetError() != AL_NO_ERROR )
	{
		__asm{ int 3 };
	}
	// cleanup
	ov_clear( &oggStream );
	oggFile.close();

	delete[] memory;

	m_filename = _filename;

	m_isEmpty = false;

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundBuffer::isStreamed() const
{
	return false;
}
//////////////////////////////////////////////////////////////////////////
ALuint ALSoundBuffer::getAlID() const
{
	return m_alID;
}
//////////////////////////////////////////////////////////////////////////
unsigned int ALSoundBuffer::decodeOggVorbis_( OggVorbis_File* stream, char* _buffer, unsigned int bufferSize )
{
	int current_section;
	long decodeSize;
	//unsigned long samplesNum;
	//short *samples;

	unsigned long bytesDone = 0;
	while (decodeSize = ov_read( stream, _buffer + bytesDone, bufferSize - bytesDone, 0, 2, 1, &current_section))
	{
		bytesDone += decodeSize;

		if ( bytesDone >= bufferSize )
		{
			break;
		}
	}

	return bytesDone;
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundBuffer::isStereo()
{
	return m_isStereo;
}
//////////////////////////////////////////////////////////////////////////
