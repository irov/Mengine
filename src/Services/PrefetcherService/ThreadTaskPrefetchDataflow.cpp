#include "ThreadTaskPrefetchDataflow.h"

#include "Interface/CodecInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"

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
    void ThreadTaskPrefetchDataflow::setDataflow( const DataflowInterfacePtr & _dataflow )
    {
        m_dataflow = _dataflow;
    }
    //////////////////////////////////////////////////////////////////////////
    const DataflowInterfacePtr & ThreadTaskPrefetchDataflow::getDataflow() const
    {
        return m_dataflow;
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
        m_dataflow = nullptr;

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

        InputStreamInterfacePtr stream = m_fileGroup->createInputFile( m_filePath, false, &m_realFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't create dataflow file '%s'"
            , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
        );

        m_stream = stream;

        DataInterfacePtr data = m_dataflow->create( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "dataflow file '%s' invalid create data"
            , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
        );

        m_data = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchDataflow::_onThreadTaskProcess()
    {
        LOGGER_INFO("prefetch", "prefetch dataflow file '%s'"
            , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
        );
        
        if( m_realFileGroup->openInputFile( m_filePath, m_stream, 0, MENGINE_UNKNOWN_SIZE, false, false ) == false )
        {
            LOGGER_ERROR( "invalid open file '%s'"
                , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
            );

            return false;
        }

        MemoryInterfacePtr memory = m_dataflow->load( m_stream, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid load file '%s'"
            , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
        );

        if( m_dataflow->isThreadFlow() == true )
        {
            if( m_dataflow->flow( m_data, memory, &m_context, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid flow file '%s'"
                    , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
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
        if( m_dataflow->isThreadFlow() == false && _successful == true )
        {
            MemoryInterfacePtr memory = std::move( m_memory );

            if( m_dataflow->flow( m_data, memory, &m_context, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid flow file '%s'"
                     , Helper::getFileGroupFullPath( this->getFileGroup(), this->getFilePath() )
                );

                return;
            }
        }

        ThreadTaskPrefetch::_onThreadTaskComplete( _successful );

        m_dataflow = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}
