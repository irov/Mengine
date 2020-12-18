#include "ThreadTaskPrefetchSoundDecoder.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchSoundDecoder::ThreadTaskPrefetchSoundDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchSoundDecoder::~ThreadTaskPrefetchSoundDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchSoundDecoder::setSoundCodec( const ConstString & _soundCodec )
    {
        m_soundCodec = _soundCodec;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ThreadTaskPrefetchSoundDecoder::getSoundCodec() const
    {
        return m_soundCodec;
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & ThreadTaskPrefetchSoundDecoder::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchSoundDecoder::_finalize()
    {
        m_soundDecoder = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchSoundDecoder::_onThreadTaskRun()
    {
        if( ThreadTaskPrefetch::_onThreadTaskRun() == false )
        {
            return false;
        }

        m_stream = m_fileGroup->createInputFile( m_filePath, false, &m_realFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_stream, "can't create input file '%s'"
            , this->getFileGroup()->getName().c_str()
        );

        m_soundDecoder = CODEC_SERVICE()
            ->createDecoderT<SoundDecoderInterfacePtr>( m_soundCodec, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_soundDecoder, "invalid create codec '%s'"
            , m_soundCodec.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchSoundDecoder::_onThreadTaskMain()
    {
        if( m_realFileGroup->openInputFile( m_filePath, m_stream, 0, 0, false, false ) == false )
        {
            LOGGER_ERROR( "invalid open file '%s:%s'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        size_t stream_size = m_stream->size();

        MemoryInputInterfacePtr memoryInput = MEMORY_SERVICE()
            ->createMemoryInput( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryInput, "file '%s:%s' invalid create memory input [size %zu]"
            , this->getFileGroup()->getName().c_str()
            , this->getFilePath().c_str()
            , stream_size
        );

        void * memory = memoryInput->newBuffer( stream_size );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "file '%s:%s' invalid alloc memory '%zu'"
            , this->getFileGroup()->getName().c_str()
            , this->getFilePath().c_str()
            , stream_size
        );

        if( m_stream->read( memory, stream_size ) != stream_size )
        {
            LOGGER_ERROR( "file '%s:%s' invalid read stream '%zu'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
                , stream_size
            );

            return false;
        }

        if( m_soundDecoder->prepareData( memoryInput ) == false )
        {
            LOGGER_ERROR( "decoder for file '%s:%s' was not initialize"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
