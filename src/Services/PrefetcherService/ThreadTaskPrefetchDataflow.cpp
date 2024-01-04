#include "ThreadTaskPrefetchDataflow.h"

#include "Interface/CodecInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchDataflow::ThreadTaskPrefetchDataflow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadTaskPrefetchDataflow::~ThreadTaskPrefetchDataflow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const DataInterfacePtr & ThreadTaskPrefetchDataflow::getData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchDataflow::setDataflowContext( const DataflowContext & _context )
    {
        m_context = _context;
    }
    //////////////////////////////////////////////////////////////////////////
    const DataflowContext & ThreadTaskPrefetchDataflow::getDataflowContext() const
    {
        return m_context;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchDataflow::_finalize()
    {
        m_data = nullptr;
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchDataflow::_onThreadTaskRun()
    {
        if( ThreadTaskPrefetch::_onThreadTaskRun() == false )
        {
            return false;
        }

        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();

        InputStreamInterfacePtr stream = fileGroup->createInputFile( filePath, false, &m_realFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create dataflow file '%s'"
            , Helper::getContentFullPath( m_content )
        );

        m_stream = stream;

        const DataflowInterfacePtr & dataflow = m_content->getDataflow();

        DataInterfacePtr data = dataflow->create( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "dataflow file '%s' invalid create data"
            , Helper::getContentFullPath( m_content )
        );

        m_data = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchDataflow::_onThreadTaskProcess()
    {
        LOGGER_INFO( "prefetch", "prefetch dataflow file '%s'"
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

        const DataflowInterfacePtr & dataflow = m_content->getDataflow();

        MemoryInterfacePtr memory = dataflow->load( m_stream, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid load file '%s'"
            , Helper::getContentFullPath( m_content )
        );

        if( dataflow->isThreadFlow() == true )
        {
            if( dataflow->flow( m_data, memory, &m_context, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid flow file '%s'"
                    , Helper::getContentFullPath( m_content )
                );

                return false;
            }
        }
        else
        {
            m_memory = memory;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchDataflow::_onThreadTaskComplete( bool _successful )
    {
        const DataflowInterfacePtr & dataflow = m_content->getDataflow();

        if( dataflow->isThreadFlow() == false && _successful == true )
        {
            MemoryInterfacePtr memory = std::move( m_memory );

            if( dataflow->flow( m_data, memory, &m_context, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid flow file '%s'"
                    , Helper::getContentFullPath( m_content )
                );

                return;
            }
        }

        ThreadTaskPrefetch::_onThreadTaskComplete( _successful );
    }
    //////////////////////////////////////////////////////////////////////////
}
