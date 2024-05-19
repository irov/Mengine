#include "AndroidHttpRequestGetAssetThreadTask.h"

#include "Environment/Android/AndroidHelper.h"

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
    AndroidHttpRequestGetAssetThreadTask::AndroidHttpRequestGetAssetThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestGetAssetThreadTask::~AndroidHttpRequestGetAssetThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequestGetAssetThreadTask::initialize( const String & _login, const String & _password, const ContentInterfacePtr & _content, const FilePath & _filePathTemp )
    {
        m_login = _login;
        m_password = _password;
        m_content = _content;
        m_filePathTemp = _filePathTemp;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequestGetAssetThreadTask::_onThreadTaskRun()
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
    jobject AndroidHttpRequestGetAssetThreadTask::_onHttp( JNIEnv * _jenv, jobject _jrequest )
    {
        jobject jkey_login = Helper::AndroidMakeJObjectString( _jenv, m_login );
        jobject jkey_password = Helper::AndroidMakeJObjectString( _jenv, m_password );

        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestGetAsset", "(Lorg/Mengine/Base/MengineHttpRequestParam;Ljava/lang/String;Ljava/lang/String;)Lorg/Mengine/Base/MengineHttpResponseParam;"
            , _jrequest
            , jkey_login
            , jkey_password
        );

        _jenv->DeleteLocalRef( jkey_login );
        _jenv->DeleteLocalRef( jkey_password );

        return jresponse;

        //MENGINE_CURLCALL( curl_easy_setopt, ( _curl, CURLOPT_CUSTOMREQUEST, "GET" ) );

        //if( m_login.empty() == false || m_password.empty() == false )
        //{
        //    const Char * login_str = m_login.c_str();
        //    const Char * password_str = m_password.c_str();

        //    MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_USERNAME, login_str) );
        //    MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_PASSWORD, password_str) );
        //}

        ///* send all data to this function  */
        //MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEFUNCTION, &Detail::cURL_WriteMemoryCallback) );

        ///* we pass our 'chunk' struct to the callback function */
        //OutputStreamInterface * stream_ptr = m_stream.get();
        //MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_WRITEDATA, (void *)stream_ptr) );

        //MENGINE_CURLCALL( curl_easy_setopt, (_curl, CURLOPT_USERAGENT, "libcurl-agent/1.0") );

        //bool OPTION_curltrace = HAS_OPTION( "curltrace" );

        //if( OPTION_curltrace == true )
        //{
        //    LOGGER_HTTP_STATISTIC( "[HTTP] get asset url '%s' login '%s' password '%s'\nfile: '%s'"
        //        , m_url.c_str()
        //        , m_login.c_str()
        //        , m_password.c_str()
        //        , Helper::getContentFullPath( m_content )
        //    );
        //}
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestGetAssetThreadTask::_onThreadTaskComplete( bool _successful )
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
            AndroidHttpRequestThreadTask::_onThreadTaskComplete( false );

            return;
        }

        if( fileGroup->moveFile( m_filePathTemp, filePath ) == false )
        {
            AndroidHttpRequestThreadTask::_onThreadTaskComplete( false );

            return;
        }

        AndroidHttpRequestThreadTask::_onThreadTaskComplete( true );
    }
    //////////////////////////////////////////////////////////////////////////
}
