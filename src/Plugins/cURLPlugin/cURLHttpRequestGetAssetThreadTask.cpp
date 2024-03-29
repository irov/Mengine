#include "cURLHttpRequestGetAssetThreadTask.h"

#include "cURLHttpErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static size_t cURL_WriteMemoryCallback( void * _contents, size_t _size, size_t _nmemb, void * _userp )
        {
            OutputStreamInterface * stream_ptr = (OutputStreamInterface *)_userp;

            size_t realsize = _size * _nmemb;

            size_t writez = stream_ptr->write( _contents, realsize );
            MENGINE_UNUSED( writez );

            return realsize;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestGetAssetThreadTask::cURLHttpRequestGetAssetThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLHttpRequestGetAssetThreadTask::~cURLHttpRequestGetAssetThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpRequestGetAssetThreadTask::initialize( const String & _login, const String & _password, const ContentInterfacePtr & _content, const FilePath & _filePathTemp )
    {
        m_login = _login;
        m_password = _password;
        m_content = _content;
        m_filePathTemp = _filePathTemp;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLHttpRequestGetAssetThreadTask::_onThreadTaskRun()
    {
        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();

        if( fileGroup->createDirectory( filePath ) == false )
        {
            return false;
        }

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( fileGroup, m_filePathTemp, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "get asset url '%s' invalid open file '%s'"
            , m_url.c_str()
            , Helper::getContentFullPath( m_content )
        );

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestGetAssetThreadTask::_onCURL( CURL * _curl )
    {
        MENGINE_CURLCALL( curl_easy_setopt, ( _curl, CURLOPT_CUSTOMREQUEST, "GET" ) );

        if( m_login.empty() == false || m_password.empty() == false )
        {
            const Char * login_str = m_login.c_str();
            const Char * password_str = m_password.c_str();

            MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_USERNAME, login_str) );
            MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_PASSWORD, password_str) );
        }

        /* send all data to this function  */
        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEFUNCTION, &Detail::cURL_WriteMemoryCallback) );

        /* we pass our 'chunk' struct to the callback function */
        OutputStreamInterface * stream_ptr = m_stream.get();
        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEDATA, (void *)stream_ptr) );

        MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_USERAGENT, "libcurl-agent/1.0") );

        bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        if( OPTION_curltrace == true )
        {
            LOGGER_HTTP_STATISTIC( "[HTTP] get asset url '%s' login '%s' password '%s'\nfile: '%s'"
                , m_url.c_str()
                , m_login.c_str()
                , m_password.c_str()
                , Helper::getContentFullPath( m_content )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLHttpRequestGetAssetThreadTask::_onThreadTaskComplete( bool _successful )
    {
        bool successful_stream_flush = true;

        FileGroupInterfacePtr fileGroup = m_content->getFileGroup();
        FilePath filePath = m_content->getFilePath();
        m_content = nullptr;

        if( m_stream != nullptr )
        {
            successful_stream_flush = m_stream->flush();

            Helper::closeOutputStreamFile( fileGroup, m_stream );
            m_stream = nullptr;
        }

        EHttpCode code = m_response->getCode();

        if( _successful == false || HTTP_CODE_IS_SUCCESSFUL( code ) == false || successful_stream_flush == false )
        {
            cURLHttpRequestThreadTask::_onThreadTaskComplete( false );

            return;
        }

        if( fileGroup->moveFile( m_filePathTemp, filePath ) == false )
        {
            cURLHttpRequestThreadTask::_onThreadTaskComplete( false );

            return;
        }

        cURLHttpRequestThreadTask::_onThreadTaskComplete( true );
    }
    //////////////////////////////////////////////////////////////////////////
}
