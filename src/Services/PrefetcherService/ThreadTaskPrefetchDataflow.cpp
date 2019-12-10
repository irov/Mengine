#include "ThreadTaskPrefetchDataflow.h"

#include "Interface/CodecInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
    bool ThreadTaskPrefetchDataflow::_onRun()
    {
        if( ThreadTaskPrefetch::_onRun() == false )
        {
            return false;
        }

        m_stream = m_fileGroup->createInputFile( m_filePath, false, &m_realFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_stream, false, "can't create input file '%s'"
            , this->getFileGroup()->getName().c_str()
        );

        m_data = m_dataflow->create( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_data, false, "dataflow '%s':'%s' invalid create data"
            , this->getFileGroup()->getName().c_str()
            , this->getFilePath().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchDataflow::_onMain()
    {
        if( m_realFileGroup->openInputFile( m_filePath, m_stream, 0, 0, false, false ) == false )
        {
            LOGGER_ERROR( "invalide open file '%s':'%s'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        MemoryInterfacePtr memory = m_dataflow->load( m_stream, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, false, "invalide load file '%s':'%s'"
            , this->getFileGroup()->getName().c_str()
            , this->getFilePath().c_str()
        );

        if( m_dataflow->isThreadFlow() == true )
        {
            if( m_dataflow->flow( m_data, memory, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalide flow file '%s':'%s'"
                    , this->getFileGroup()->getName().c_str()
                    , this->getFilePath().c_str()
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
    void ThreadTaskPrefetchDataflow::_onComplete( bool _successful )
    {
        if( m_dataflow->isThreadFlow() == false && _successful == true )
        {
            MemoryInterfacePtr memory = std::move( m_memory );

            if( m_dataflow->flow( m_data, memory, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalide flow file '%s':'%s'"
                    , this->getFileGroup()->getName().c_str()
                    , this->getFilePath().c_str()
                );

                return;
            }
        }

        ThreadTaskPrefetch::_onComplete( _successful );

        m_dataflow = nullptr;
    }
}
