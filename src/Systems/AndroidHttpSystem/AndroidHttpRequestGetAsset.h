#pragma once

#include "AndroidHttpRequest.h"

#include "Interface/OutputStreamInterface.h"
#include "Interface/ContentInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestGetAsset
        : public AndroidHttpRequest
    {
        DECLARE_FACTORABLE( AndroidHttpRequestGetAsset );

    public:
        AndroidHttpRequestGetAsset();
        ~AndroidHttpRequestGetAsset() override;

    public:
        void setLogin( const String & _login );
        const String & getLogin() const;

        void setPassword( const String & _password );
        const String & getPassword() const;

        void setContent( const ContentInterfacePtr & _content );
        const ContentInterfacePtr & getContent() const;

    protected:
        bool _onThreadTaskRun() override;

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        String m_login;
        String m_password;
        ContentInterfacePtr m_content;

        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestGetAsset, HttpRequestInterface> AndroidHttpRequestGetAssetPtr;
    //////////////////////////////////////////////////////////////////////////
}