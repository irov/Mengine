#include "SoundDecoderWAV.h"

#include "Interface/FileGroupInterface.h"

#include "Kernel/Magic.h"
#include "Kernel/Logger.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct WAVE_HEADER
    {
        Char ChunkID[4];
        uint32_t ChunkSize;
        Char Format[4];
        Char Subchunk1ID[4];
        uint32_t Subchunk1Size;
        uint16_t AudioFormat;
        uint16_t NumChannels;
        uint32_t SampleRate;
        uint32_t ByteRate;
        uint16_t BlockAlign;
        uint16_t BitsPerSample;
    };
    //////////////////////////////////////////////////////////////////////////
    SoundDecoderWAV::SoundDecoderWAV()
        : m_chunkDataPos( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SoundDecoderWAV::~SoundDecoderWAV()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderWAV::_prepareData()
    {
        WAVE_HEADER header;

        const InputStreamInterfacePtr & stream = this->getStream();

        stream->read( header.ChunkID, 4 );

        if( Helper::magicTest4( header.ChunkID, "RIFF" ) == false )
        {
            LOGGER_ERROR( "sound decoder WAV invalid chunk id 'RIFF'" );

            return false;
        }

        stream->read( &header.ChunkSize, sizeof( header.ChunkSize ) );

        stream->read( &header.Format, 4 );

        if( Helper::magicTest4( header.Format, "WAVE" ) == false )
        {
            LOGGER_ERROR( "sound decoder WAV invalid format 'WAVE'" );

            return false;
        }

        stream->read( &header.Subchunk1ID, 4 );

        if( Helper::magicTest4( header.Subchunk1ID, "fmt " ) == false )
        {
            LOGGER_ERROR( "sound decoder WAV invalid subchunk id 'fmt '" );

            return false;
        }

        stream->read( &header.Subchunk1Size, sizeof( header.Subchunk1Size ) );

        stream->read( &header.AudioFormat, sizeof( header.AudioFormat ) );

        if( header.AudioFormat != 1 )
        {
            LOGGER_ERROR( "sound decoder WAV invalid AudioFormat %u"
                , header.AudioFormat
            );

            return false;
        }

        stream->read( &header.NumChannels, sizeof( header.NumChannels ) );
        stream->read( &header.SampleRate, sizeof( header.SampleRate ) );
        stream->read( &header.ByteRate, sizeof( header.ByteRate ) );
        stream->read( &header.BlockAlign, sizeof( header.BlockAlign ) );
        stream->read( &header.BitsPerSample, sizeof( header.BitsPerSample ) );

        if( (header.BitsPerSample != 16) && (header.BitsPerSample != 8) )
        {
            LOGGER_ERROR( "sound decoder WAV invalid bits per sample %u"
                , header.BitsPerSample
            );

            return false;
        }

        stream->skip( header.Subchunk1Size - 16 );

        size_t chunkDataSize;
        if( this->findChunkData_( stream, &chunkDataSize ) == false )
        {
            LOGGER_ERROR( "sound decoder WAV invalid find chunk data [bps %u]"
                , header.BitsPerSample
            );

            return false;
        }

        m_chunkDataPos = (uint32_t)stream->tell();

        m_dataInfo.size = chunkDataSize;
        m_dataInfo.channels = header.NumChannels;
        m_dataInfo.frequency = header.SampleRate;
        m_dataInfo.bits = header.BitsPerSample / 8;

        m_dataInfo.duration = (float)(m_dataInfo.size * 1000 / (m_dataInfo.frequency * m_dataInfo.channels * m_dataInfo.bits));

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderWAV::findChunkData_( const InputStreamInterfacePtr & _stream, size_t * const _size )
    {
        while( _stream->eof() == false )
        {
            Char Subchunk2ID[4];
            _stream->read( &Subchunk2ID, 4 );

            uint32_t Subchunk2Size;
            _stream->read( &Subchunk2Size, sizeof( Subchunk2Size ) );

            if( Helper::magicTest4( Subchunk2ID, "data" ) == true )
            {
                *_size = Subchunk2Size;

                return true;
            }

            _stream->skip( Subchunk2Size );
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t SoundDecoderWAV::_decode( const DecoderData * _decoderData )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _decoderData, "invalid decode data" );
        MENGINE_ASSERTION_TYPE( _decoderData, const SoundDecoderData *, "invalid decode data type" );

        MENGINE_PROFILER_CATEGORY();

        void * buffer = _decoderData->buffer;
        size_t size = _decoderData->size;

        const InputStreamInterfacePtr & stream = this->getStream();

        size_t bytesDone = stream->read( buffer, size );

        return bytesDone;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SoundDecoderWAV::_seek( float _time )
    {
        if( _time > m_dataInfo.duration )
        {
            LOGGER_ERROR( "sound decoder WAV timing %f > total %f"
                , _time
                , m_dataInfo.duration
            );

            _time = m_dataInfo.duration;
        }

        size_t wav_carriage = ((size_t)(_time) * (m_dataInfo.frequency * m_dataInfo.channels * m_dataInfo.bits)) / 1000;

        const InputStreamInterfacePtr & stream = this->getStream();

        bool result = stream->seek( m_chunkDataPos + wav_carriage );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    float SoundDecoderWAV::_tell() const
    {
        const InputStreamInterfacePtr & stream = this->getStream();

        size_t position = stream->tell();

        float length = (float)((position - m_chunkDataPos) * 1000.f / float( m_dataInfo.frequency * m_dataInfo.channels * m_dataInfo.bits ));

        return length;
    }
    //////////////////////////////////////////////////////////////////////////
}
