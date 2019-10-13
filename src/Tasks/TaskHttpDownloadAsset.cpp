#include "TaskHttpDownloadAsset.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskHttpDownloadAsset::TaskHttpDownloadAsset( String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const LambdacURLReceiver & _lambda )
        : TaskHttpBase( _url, _timeout, _lambda )
        , m_login( _login )
        , m_password( _password )
        , m_fileGroup( _fileGroup )
        , m_filePath( _filePath )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskHttpDownloadAsset::~TaskHttpDownloadAsset()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskHttpDownloadAsset::_onCheck()
    {
        if( m_fileGroup->existFile( m_filePath, true ) == true )
        {
            LOGGER_MESSAGE( "url '%s' category '%s' file alredy exist '%s'"
                , m_url.c_str()
                , m_fileGroup->getName().c_str()
                , m_filePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskHttpDownloadAsset::_onRun()
    {
        cURLReceiverInterfacePtr receiver = this->createHttpReceiver_();

        HttpRequestID requestId = CURL_SERVICE()
            ->downloadAsset( m_url, m_login, m_password, m_fileGroup, m_filePath, m_timeout, receiver );

        if( requestId == 0 )
        {
            LOGGER_ERROR( "error http event" );

            return true;
        }

        m_requestId = requestId;

        return false;
    }
}