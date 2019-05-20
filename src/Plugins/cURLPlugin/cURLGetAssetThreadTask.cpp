#include "cURLGetAssetThreadTask.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLGetAssetThreadTask::cURLGetAssetThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLGetAssetThreadTask::~cURLGetAssetThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLGetAssetThreadTask::initialize( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filepath )
    {
        m_url = _url;
        m_login = _login;
        m_password = _password;
        m_fileGroup = _fileGroup;
        m_filePath = _filepath;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLGetAssetThreadTask::_onRun()
    {
        if( m_fileGroup->createDirectory( m_filePath ) == false )
        {
            return false;
        }

        m_stream = FILE_SERVICE()
            ->openOutputFile( m_fileGroup, m_filePath, MENGINE_DOCUMENT_FUNCTION );

        if( m_stream == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static size_t WriteMemoryCallback( void * _contents, size_t _size, size_t _nmemb, void * _userp )
    {
        OutputStreamInterface * stream_ptr = (OutputStreamInterface *)_userp;

        size_t realsize = _size * _nmemb;

        stream_ptr->write( _contents, realsize );

        return realsize;
    }
    //////////////////////////////////////////////////////////////////////////
    static int32_t XFERInfoCallback( void * _userp, curl_off_t _dltotal, curl_off_t _dlnow, curl_off_t _ultotal, curl_off_t _ulnow )
    {
        MENGINE_UNUSED( _dltotal );
        MENGINE_UNUSED( _dlnow );
        MENGINE_UNUSED( _ultotal );
        MENGINE_UNUSED( _ulnow );

        cURLGetAssetThreadTask * task = (cURLGetAssetThreadTask *)_userp;

        if( task->isCancel() == true )
        {
            return 1;
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLGetAssetThreadTask::_onCURL( CURL * _curl )
    {
        curl_easy_setopt( _curl, CURLOPT_URL, m_url.c_str() );

        curl_easy_setopt( _curl, CURLOPT_USERNAME, m_login.c_str() );
        curl_easy_setopt( _curl, CURLOPT_PASSWORD, m_password.c_str() );

        /* send all data to this function  */
        curl_easy_setopt( _curl, CURLOPT_WRITEFUNCTION, &WriteMemoryCallback );

        /* we pass our 'chunk' struct to the callback function */
        OutputStreamInterface * stream_ptr = m_stream.get();
        curl_easy_setopt( _curl, CURLOPT_WRITEDATA, (void *)stream_ptr );

        curl_easy_setopt( _curl, CURLOPT_XFERINFOFUNCTION, &XFERInfoCallback );
        curl_easy_setopt( _curl, CURLOPT_XFERINFODATA, (void *)this );

        /* some servers don't like requests that are made without a user-agent
        field, so we provide one */
        //curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        curl_easy_setopt( _curl, CURLOPT_NOPROGRESS, 0L );

        if( CONFIG_VALUE( "HTTP", "Log", false ) == true )
        {
            LOGGER_STATISTIC( "HTTP: get asset url '%s' login '%s' password '%s'\nfile: '%s:%s'"
                , m_url.c_str()
                , m_login.c_str()
                , m_password.c_str()
                , m_fileGroup->getName().c_str()
                , m_filePath.c_str()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLGetAssetThreadTask::_onComplete( bool _successful )
    {
        m_stream->flush();
        m_stream = nullptr;

        cURLThreadTask::_onComplete( _successful );
    }
}
