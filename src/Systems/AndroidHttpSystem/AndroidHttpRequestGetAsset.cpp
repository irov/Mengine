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
        : AndroidHttpRequest( false )
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

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( fileGroup, filePath, true, MENGINE_DOCUMENT_FACTORABLE );

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
        jobject jobject_login = Helper::AndroidMakeJObjectString( _jenv, m_login );
        jobject jobject_password = Helper::AndroidMakeJObjectString( _jenv, m_password );

        jobject jresponse = Helper::AndroidCallObjectStaticClassMethod( _jenv, "org/Mengine/Base/MengineNetwork", "httpRequestGetAsset", "(Lorg/Mengine/Base/MengineParamHttpRequest;Ljava/lang/String;Ljava/lang/String;)Lorg/Mengine/Base/MengineParamHttpResponse;"
            , _jrequest
            , jobject_login
            , jobject_password
        );

        Mengine_JNI_DeleteLocalRef( _jenv, jobject_login );
        Mengine_JNI_DeleteLocalRef( _jenv, jobject_password );

        return jresponse;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidHttpRequestGetAsset::_onThreadTaskComplete( bool _successful )
    {
        const HttpResponseInterfacePtr & response = this->getReponse();

        EHttpCode code = response->getCode();

        bool successful_asset = _successful;

        if( HTTP_CODE_IS_SUCCESSFUL( code ) == false )
        {
            successful_asset = false;
        }

        FileGroupInterfacePtr fileGroup = m_content->getFileGroup();
        m_content = nullptr;

        if( m_stream != nullptr )
        {
            if( successful_asset == true )
            {
                const Data & data = response->getData();

                Helper::writeStreamData( m_stream, data );

                if( m_stream->flush() == false )
                {
                    successful_asset = false;
                }
            }

            if( Helper::closeOutputStreamFile( fileGroup, m_stream, successful_asset ) == false )
            {
                successful_asset = false;
            }

            m_stream = nullptr;
        }

        AndroidHttpRequest::_onThreadTaskComplete( successful_asset );
    }
    //////////////////////////////////////////////////////////////////////////
}
