#include "cURLGetAssetThreadTask.h"

#include "cURLErrorHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"

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
    cURLGetAssetThreadTask::cURLGetAssetThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLGetAssetThreadTask::~cURLGetAssetThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLGetAssetThreadTask::initialize( const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filepath, const FilePath & _filePathTemp )
    {
        m_login = _login;
        m_password = _password;
        m_fileGroup = _fileGroup;
        m_filePath = _filepath;
        m_filePathTemp = _filePathTemp;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLGetAssetThreadTask::_onThreadTaskRun()
    {
        if( m_fileGroup->createDirectory( m_filePath ) == false )
        {
            return false;
        }

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, m_filePathTemp, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "get asset url '%s' invalid open file '%s'"
            , m_url.c_str()
            , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
        );

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLGetAssetThreadTask::_onCURL( CURL * _curl )
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
            LOGGER_STATISTIC( "HTTP: get asset url '%s' login '%s' password '%s'\nfile: '%s'"
                , m_url.c_str()
                , m_login.c_str()
                , m_password.c_str()
                , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLGetAssetThreadTask::_onThreadTaskComplete( bool _successful )
    {
        bool successful_stream_flush = true;

        if( m_stream != nullptr )
        {
            successful_stream_flush = m_stream->flush();

            Helper::closeOutputStreamFile( m_fileGroup, m_stream );
            m_stream = nullptr;
        }

        uint32_t code = m_response->getCode();

        if( _successful == false || code != 200 || successful_stream_flush == false )
        {
            m_fileGroup = nullptr;

            cURLThreadTask::_onThreadTaskComplete( false );

            return;
        }

        if( m_fileGroup->moveFile( m_filePathTemp, m_filePath ) == false )
        {
            m_fileGroup = nullptr;

            cURLThreadTask::_onThreadTaskComplete( false );

            return;
        }

        m_fileGroup = nullptr;

        cURLThreadTask::_onThreadTaskComplete( true );
    }
    //////////////////////////////////////////////////////////////////////////
}
