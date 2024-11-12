#include "AndroidHttpRequestGetAsset.h"

#include "Environment/Android/AndroidHelper.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/StreamHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestGetAsset::AndroidHttpRequestGetAsset()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidHttpRequestGetAsset::~AndroidHttpRequestGetAsset()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestGetAsset::setLogin( const String & _login )
    {
        m_login = _login;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & AndroidHttpRequestGetAsset::getLogin() const
    {
        return m_login;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestGetAsset::setPassword( const String & _password )
    {
        m_password = _password;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & AndroidHttpRequestGetAsset::getPassword() const
    {
        return m_password;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestGetAsset::setContent( const ContentInterfacePtr & _content )
    {
        m_content = _content;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & AndroidHttpRequestGetAsset::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidHttpRequestGetAsset::_onThreadTaskRun()
    {
        const FileGroupInterfacePtr & fileGroup = m_content->getFileGroup();
        const FilePath & filePath = m_content->getFilePath();

        if( fileGroup->createDirectory( filePath ) == false )
        {
            return false;
        }

        FilePath filePathTmp = Helper::stringizeFilePathFormat( "%s.~tmp", filePath.c_str() );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( fileGroup, filePathTmp, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "get asset url '%s' invalid open file '%s'"
            , m_url.c_str()
            , Helper::getContentFullPath( m_content ).c_str()
        );

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jobject AndroidHttpRequestGetAsset::_onHttp( JNIEnv * _jenv, jobject _jrequest )
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
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestGetAsset::_onThreadTaskComplete( bool _successful )
    {
        const HttpResponseInterfacePtr & response = this->getReponse();

        bool successful_stream_flush = true;

        FileGroupInterfacePtr fileGroup = m_content->getFileGroup();
        FilePath filePath = m_content->getFilePath();
        m_content = nullptr;

        if( m_stream != nullptr )
        {
            const Data & data = response->getData();

            Helper::writeStreamData( m_stream, data );

            successful_stream_flush = m_stream->flush();

            Helper::closeOutputStreamFile( fileGroup, m_stream );
            m_stream = nullptr;
        }

        EHttpCode code = response->getCode();

        if( _successful == false || HTTP_CODE_IS_SUCCESSFUL( code ) == false || successful_stream_flush == false )
        {
            AndroidHttpRequest::_onThreadTaskComplete( false );

            return;
        }

        FilePath filePathTmp = Helper::stringizeFilePathFormat( "%s.~tmp", filePath.c_str() );

        if( fileGroup->moveFile( filePathTmp, filePath ) == false )
        {
            AndroidHttpRequest::_onThreadTaskComplete( false );

            return;
        }

        AndroidHttpRequest::_onThreadTaskComplete( true );
    }
    //////////////////////////////////////////////////////////////////////////
}
