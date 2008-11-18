
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
void ALSoundBuffer::addSource( Menge::SoundSourceInterface *_source)
{
	m_sources.push_back(_source);
}
//////////////////////////////////////////////////////////////////////////
void ALSoundBuffer::removeSource( Menge::SoundSourceInterface *_source)
{
	TVectorSoundSourceInterface::iterator it_find = 
		std::find(m_sources.begin(), m_sources.end(), _source);

	if( it_find != m_sources.end() )
	{
		m_sources.erase(it_find);
	}
}
//////////////////////////////////////////////////////////////////////////
bool ALSoundBuffer::loadOgg( Menge::DataStreamInterface* _stream )
{
	alGetError();	// clear errors

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
	vorbisCallbacks.read_func  = s_readOgg_;
	vorbisCallbacks.seek_func  = s_seekOgg_;
	vorbisCallbacks.tell_func  = s_tellOgg_;
	vorbisCallbacks.close_func = s_closeOgg_;

	OggVorbis_File oggStream;

	// Open the file from memory.  We need to pass it a pointer to our data (in this case our SOggFile structure),
	// a pointer to our ogg stream (which the vorbis libs will fill up for us), and our callbacks
	if ( ov_open_callbacks( _stream, &oggStream, NULL, 0, vorbisCallbacks ) < 0 )
	{
		return false;
	}

	int seek = 0;

	ogg_int64_t pcmlength = ov_pcm_total( &oggStream, -1 );
	m_lenghtMs = ov_time_total(&oggStream, -1) * 1000;
	vorbis_info *vi = ov_info( &oggStream, -1 );

	unsigned int memSize = (unsigned int)pcmlength * 2 * vi->channels;
	char *memory = new char[memSize];

	int eof = 0;

	seek = decodeOggVorbis_( &oggStream, memory, memSize );

	// ������ ������ � ������ OGG
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

	// ��������� �� ������ ������ �����
	ALvoid *data = memory;

	// ������ ����� �������
	ALsizei size = seek;

	// ������� ����� � ������
	ALsizei freq = vi->rate;

	alBufferData( m_alID, format, data, size, freq );

	// cleanup
	ov_clear( &oggStream );

	delete[] memory;

	m_isEmpty = false;

	return true;
}
//////////////////////////////////////////////////////////////////////////
const Menge::String & ALSoundBuffer::getFilename()
{ 
	return m_filename; 
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
int ALSoundBuffer::getPosMs()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void ALSoundBuffer::setPosMs( float _posMs )
{

}
//////////////////////////////////////////////////////////////////////////
