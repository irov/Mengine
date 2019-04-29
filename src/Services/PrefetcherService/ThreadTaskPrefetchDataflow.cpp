#include "ThreadTaskPrefetchDataflow.h"

#include "Interface/CodecInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

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

        m_stream = m_fileGroup->createInputFile( m_filePath, false, MENGINE_DOCUMENT_FUNCTION );

        if( m_stream == nullptr )
        {
            LOGGER_ERROR( "can't create input file '%s'"
                , this->getFileGroup()->getName().c_str()
            );

            return false;
        }

        m_data = m_dataflow->create( MENGINE_DOCUMENT_FUNCTION );

        if( m_data == nullptr )
        {
            LOGGER_ERROR( "dataflow '%s':'%s' invalid create data"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTaskPrefetchDataflow::_onMain()
    {
        if( m_fileGroup->openInputFile( m_filePath, m_stream, 0, 0, false ) == false )
        {
            LOGGER_ERROR( "invalide open file '%s':'%s'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        MemoryInterfacePtr memory = m_dataflow->load( m_stream, m_filePath.c_str() );

        if( memory == nullptr )
        {
            LOGGER_ERROR( "invalide load file '%s':'%s'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        if( m_dataflow->isThreadFlow() == true )
        {
            if( m_dataflow->flow( m_data, memory, m_filePath.c_str() ) == false )
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

            if( m_dataflow->flow( m_data, memory, m_filePath.c_str() ) == false )
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
