#include "ThreadTaskPrefetchSoundDecoder.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

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
    const SoundDecoderInterfacePtr & ThreadTaskPrefetchSoundDecoder::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchSoundDecoder::_finalize()
    {
        m_soundDecoder = nullptr;
        m_memoryInput = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchSoundDecoder::_onThreadTaskRun()
    {
        if( ThreadTaskPrefetch::_onThreadTaskRun() == false )
        {
            return false;
        }

        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();

        InputStreamInterfacePtr stream = fileGroup->createInputFile( filePath, false, &m_realFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create sound file '%s'"
            , Helper::getContentFullPath( m_content )
        );

        m_stream = stream;

        const ConstString & codecType = m_content->getCodecType();

        SoundDecoderInterfacePtr soundDecoder = CODEC_SERVICE()
            ->createDecoder( codecType, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( soundDecoder, "invalid create codec '%s'"
            , m_content->getCodecType().c_str()
        );

        m_soundDecoder = soundDecoder;

        MemoryInputInterfacePtr memoryInput = MEMORY_SERVICE()
            ->createMemoryInput( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryInput, "invalid create memory" );

        m_memoryInput = memoryInput;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchSoundDecoder::_onThreadTaskProcess()
    {
        LOGGER_INFO("prefetch", "prefetch sound file '%s'"
            , Helper::getContentFullPath( m_content )
        );

        const FilePath & filePath = m_content->getFilePath();
        
        if( m_realFileGroup->openInputFile( filePath, m_stream, 0, MENGINE_UNKNOWN_SIZE, false, false ) == false )
        {
            LOGGER_ERROR( "invalid open file '%s'"
                , Helper::getContentFullPath( m_content )
            );

            return false;
        }

        size_t stream_size = m_stream->size();

        void * memory = m_memoryInput->newBuffer( stream_size );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "file '%s' invalid alloc memory '%zu'"
            , Helper::getContentFullPath( m_content )
            , stream_size
        );

        if( m_stream->read( memory, stream_size ) != stream_size )
        {
            LOGGER_ERROR( "file '%s' invalid read stream '%zu'"
                , Helper::getContentFullPath( m_content )
                , stream_size
            );

            return false;
        }

        if( m_soundDecoder->prepareData( m_memoryInput ) == false )
        {
            LOGGER_ERROR( "decoder for file '%s' was not initialize"
                , Helper::getContentFullPath( m_content )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
