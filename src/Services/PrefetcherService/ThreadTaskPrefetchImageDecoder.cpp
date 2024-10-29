#include "ThreadTaskPrefetchImageDecoder.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

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

        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();

        InputStreamInterfacePtr stream = fileGroup->createInputFile( filePath, false, &m_realFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create image file '%s'"
            , Helper::getContentFullPath( m_content ).c_str()
        );

        m_stream = stream;

        const ConstString & codecType = m_content->getCodecType();

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imageDecoder, "invalid create codec '%s'"
            , m_content->getCodecType().c_str()
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
        LOGGER_INFO( "prefetch", "prefetch image file '%s' codec '%s'"
            , Helper::getContentFullPath( m_content ).c_str()
            , m_content->getCodecType().c_str()
        );

        const FilePath & filePath = m_content->getFilePath();

        if( m_realFileGroup->openInputFile( filePath, m_stream, 0, MENGINE_UNKNOWN_SIZE, false, false ) == false )
        {
            LOGGER_ERROR( "invalid open file '%s' codec '%s'"
                , Helper::getContentFullPath( m_content ).c_str()
                , m_content->getCodecType().c_str()
            );

            return false;
        }

        MENGINE_ASSERTION_FATAL( m_stream->size() != 0, "empty file '%s' codec '%s'"
            , Helper::getContentFullPath( m_content ).c_str()
            , m_content->getCodecType().c_str()
        );

        size_t stream_size = m_stream->size();

        void * memory = m_memoryInput->newBuffer( stream_size );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "file '%s' codec '%s' invalid alloc memory '%zu'"
            , Helper::getContentFullPath( m_content ).c_str()
            , m_content->getCodecType().c_str()
            , stream_size
        );

        if( m_stream->read( memory, stream_size ) != stream_size )
        {
            LOGGER_ERROR( "file '%s' codec '%s' invalid read stream '%zu'"
                , Helper::getContentFullPath( m_content ).c_str()
                , m_content->getCodecType().c_str()
                , stream_size
            );

            return false;
        }

        if( m_imageDecoder->prepareData( m_memoryInput ) == false )
        {
            LOGGER_ERROR( "decoder for file '%s' codec '%s' was not initialize"
                , Helper::getContentFullPath( m_content ).c_str()
                , m_content->getCodecType().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
