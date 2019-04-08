#include "ThreadTaskPrefetchSoundDecoder.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/FactorableUnique.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

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
    bool ThreadTaskPrefetchSoundDecoder::_onRun()
    {
        if( ThreadTaskPrefetch::_onRun() == false )
        {
            return false;
        }

        m_stream = m_fileGroup->createInputFile( m_filePath, false, MENGINE_DOCUMENT_FUNCTION );

        if( m_stream == nullptr )
        {
            LOGGER_ERROR( "can't create input file '%s'"
                , this->getFileGroup()->getName().c_str()
            );

            return false;
        }

        m_soundDecoder = CODEC_SERVICE()
            ->createDecoderT<SoundDecoderInterfacePtr>( m_soundCodec, MENGINE_DOCUMENT_FUNCTION );

        if( m_soundDecoder == nullptr )
        {
            LOGGER_ERROR( "invalide create codec '%s'"
                , m_soundCodec.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchSoundDecoder::_onMain()
    {
        if( m_fileGroup->openInputFile( m_filePath, m_stream, 0, 0, false ) == false )
        {
            LOGGER_ERROR( "invalide open file '%s:%s'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        size_t stream_size = m_stream->size();

        MemoryInputInterfacePtr memoryInput = MEMORY_SERVICE()
            ->createMemoryInput( MENGINE_DOCUMENT_FUNCTION );

        if( memoryInput == nullptr )
        {
            LOGGER_ERROR( "file '%s:%s' invalid create memory input"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
                , stream_size
            );

            return false;
        }

        void * memory = memoryInput->newBuffer( stream_size );

        if( memory == nullptr )
        {
            LOGGER_ERROR( "file '%s:%s' invalid alloc memory '%d'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
                , stream_size
            );

            return false;
        }

        if( m_stream->read( memory, stream_size ) != stream_size )
        {
            LOGGER_ERROR( "file '%s:%s' invalid read stream '%d'"
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
}
