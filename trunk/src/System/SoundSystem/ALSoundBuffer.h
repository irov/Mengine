
#pragma once

#	include "AL/AL.h"

#	include "../../Interface/SoundSystemInterface.h"

#	include <vector>
#	include "vorbis/vorbisfile.h"

class ALSoundBuffer 
	: public Menge::SoundBufferInterface
{
public:
	ALSoundBuffer();
	virtual ~ALSoundBuffer();

	virtual bool isStreamed()	const;
	ALuint getAlID()			const;
	unsigned int  getLenghtMs()	const;

	void addSource( Menge::SoundSourceInterface* _source);
	void removeSource( Menge::SoundSourceInterface* _source);

	virtual bool loadOgg( Menge::DataStreamInterface* _stream );
	const Menge::String & getFilename();

	bool isStereo();

	virtual int getPosMs();
	virtual void setPosMs( float _posMs );

	static unsigned int decodeOggVorbis_( OggVorbis_File* stream, char* _buffer, unsigned int _bufferSize );
protected:

	typedef std::vector<Menge::SoundSourceInterface*>	TVectorSoundSourceInterface;
	TVectorSoundSourceInterface m_sources;
	ALuint m_alID;
	unsigned int m_lenghtMs;
	Menge::String m_filename;
	bool m_isEmpty;
	bool m_isStereo;

};