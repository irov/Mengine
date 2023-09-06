#include "ThreadTaskPrefetchImageDecoder.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileGroupHelper.h"

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
    void ThreadTaskPrefetchImageDecoder::setCodecType( const ConstString & _codecType )
    {
        m_codecType = _codecType;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ThreadTaskPrefetchImageDecoder::getCodecType() const
    {
        return m_codecType;
    }
    //////////////////////////////////////////////////////////////////////////
    const ImageDecoderInterfacePtr & ThreadTaskPrefetchImageDecoder::getDecoder() const
    {
        return m_imageDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchImageDecoder::_finalize()
    {
        m_imageDecoder = nullptr;
        m_memoryInput = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchImageDecoder::_onThreadTaskRun()
    {
        if( ThreadTaskPrefetch::_onThreadTaskRun() == false )
        {
            return false;
        }

        InputStreamInterfacePtr stream = m_fileGroup->createInputFile( m_filePath, false, &m_realFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create image file '%s'"
            , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
        );

        m_stream = stream;

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoder( m_codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imageDecoder, "invalid create codec '%s'"
            , m_codecType.c_str()
        );

        m_imageDecoder = imageDecoder;

        MemoryInputInterfacePtr memoryInput = MEMORY_SERVICE()
            ->createMemoryInput( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryInput, "invalid create memory" );

        m_memoryInput = memoryInput;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchImageDecoder::_onThreadTaskProcess()
    {
        LOGGER_INFO("prefetch", "prefetch image file '%s' codec '%s'"
            , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
            , this->getCodecType().c_str()
        );
        
        if( m_realFileGroup->openInputFile( m_filePath, m_stream, 0, MENGINE_UNKNOWN_SIZE, false, false ) == false )
        {
            LOGGER_ERROR( "invalid open file '%s' codec '%s'"
                , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
                , this->getCodecType().c_str()
            );

            return false;
        }

        MENGINE_ASSERTION_FATAL( m_stream->size() != 0, "empty file '%s' codec '%s'"
            , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
            , this->getCodecType().c_str()
        );

        size_t stream_size = m_stream->size();

        void * memory = m_memoryInput->newBuffer( stream_size );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "file '%s' codec '%s' invalid alloc memory '%zu'"
            , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
            , this->getCodecType().c_str()
            , stream_size
        );

        if( m_stream->read( memory, stream_size ) != stream_size )
        {
            LOGGER_ERROR( "file '%s' codec '%s' invalid read stream '%zu'"
                , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
                , this->getCodecType().c_str()
                , stream_size
            );

            return false;
        }

        if( m_imageDecoder->prepareData( m_memoryInput ) == false )
        {
            LOGGER_ERROR( "decoder for file '%s' codec '%s' was not initialize"
                , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
                , this->getCodecType().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
