#	include "SDLSoundBufferMemory.h"
#	include "SDLSoundError.h"

#	include "Core/MemoryHelper.h"

#	include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLSoundBufferMemory::SDLSoundBufferMemory()
        : m_frequency(0)
        , m_channels(0)
        , m_length(0.0f)
    {
        memset(&m_chunk, 0, sizeof(m_chunk));
        m_chunk.volume = 128;
    }
    //////////////////////////////////////////////////////////////////////////
    SDLSoundBufferMemory::~SDLSoundBufferMemory()
    {
        stdex_free(m_chunk.abuf);
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLSoundBufferMemory::load(const SoundDecoderInterfacePtr & _soundDecoder)
    {
        bool result = SDLSoundBuffer::load(_soundDecoder);

        if( result )
        {
            const SoundCodecDataInfo* dataInfo = m_soundDecoder->getCodecDataInfo();

            m_frequency = dataInfo->frequency;
            m_channels = dataInfo->channels;
            m_length = dataInfo->length;
            size_t size = dataInfo->size;

            MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheBuffer( m_serviceProvider, size, __FILE__, __LINE__ );

            if( binary_buffer == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("SDLSoundBufferMemory::load invalid sound %d memory %d"
                                                , size
                                                );

                return false;
            }

            void * binary_memory = binary_buffer->getMemory();

            size_t decode_size = m_soundDecoder->decode(binary_memory, size);

            if( decode_size == 0 )
            {
                LOGGER_ERROR(m_serviceProvider)("SDLSoundBufferMemory::load invalid sound %d decode %d"
                                                , size
                                                );

                return false;
            }

            decode_size -= decode_size & 3;

            m_chunk.abuf = reinterpret_cast<Uint8*>(stdex_malloc(decode_size));
            memcpy(m_chunk.abuf, binary_memory, decode_size);
            m_chunk.alen = static_cast<Uint32>(decode_size);
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLSoundBufferMemory::play(int _channel, bool _loop)
    {
        Mix_PlayChannelTimed(_channel, &m_chunk, _loop ? -1 : 0, -1);
    }
}   // namespace Menge
