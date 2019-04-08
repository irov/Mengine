#include "ThreadTaskPrefetchImageDecoder.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/FactorableUnique.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchImageDecoder::ThreadTaskPrefetchImageDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchImageDecoder::~ThreadTaskPrefetchImageDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchImageDecoder::setImageCodec( const ConstString & _codec )
    {
        m_imageCodec = _codec;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ThreadTaskPrefetchImageDecoder::getImageCodec() const
    {
        return m_imageCodec;
    }
    //////////////////////////////////////////////////////////////////////////
    const ImageDecoderInterfacePtr & ThreadTaskPrefetchImageDecoder::getDecoder() const
    {
        return m_imageDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchImageDecoder::_onRun()
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

        m_imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( m_imageCodec, MENGINE_DOCUMENT_FUNCTION );

        if( m_imageDecoder == nullptr )
        {
            LOGGER_ERROR( "invalide create codec %s"
                , m_imageCodec.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchImageDecoder::_onMain()
    {
        if( m_fileGroup->openInputFile( m_filePath, m_stream, 0, 0, false ) == false )
        {
            LOGGER_ERROR( "invalide open file '%s:%s'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        MemoryInputInterfacePtr memoryInput = MEMORY_SERVICE()
            ->createMemoryInput( MENGINE_DOCUMENT_FUNCTION );

        if( memoryInput == nullptr )
        {
            LOGGER_ERROR( "file '%s:%s' invalide create memory input"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        size_t stream_size = m_stream->size();

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

        if( m_imageDecoder->prepareData( memoryInput ) == false )
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
