#include "ThreadTaskPrefetchDataflow.h"

#include "Interface/CodecInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/Logger.h"

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

        m_stream = m_fileGroup->createInputFile( m_filePath, false );

        if( m_stream == nullptr )
        {
            LOGGER_ERROR( "ThreadTaskPrefetchDataflow::_onRun can't create input file '%s'"
                , this->getFileGroup()->getName().c_str()
            );

            return false;
        }

        m_data = m_dataflow->create();

        if( m_data == nullptr )
        {
            LOGGER_ERROR( "ThreadTaskPrefetchDataflow::_onRun: '%s':'%s' dataflow invalid create data"
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
            LOGGER_ERROR( "ThreadTaskPrefetcherTextureDecoder::_onRun: invalide open file '%s':'%s'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        if( m_dataflow->load( m_data, m_stream ) == false )
        {
            LOGGER_ERROR( "ThreadTaskPrefetcherTextureDecoder::_onRun: invalide load file '%s':'%s'"
                , this->getFileGroup()->getName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTaskPrefetchDataflow::_onComplete( bool _successful )
    {
        ThreadTaskPrefetch::_onComplete( _successful );

        m_dataflow = nullptr;
    }
}
