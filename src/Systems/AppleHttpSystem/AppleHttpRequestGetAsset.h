#pragma once

#include "AppleHttpRequest.h"

#include "Interface/OutputStreamInterface.h"
#include "Interface/ContentInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestGetAsset
        : public AppleHttpRequest
    {
        DECLARE_FACTORABLE( AppleHttpRequestGetAsset );

    public:
        AppleHttpRequestGetAsset();
        ~AppleHttpRequestGetAsset() override;

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
        AppleHttpResponseParam * _onHttp( AppleHttpRequestParam * request ) override;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        String m_login;
        String m_password;
        ContentInterfacePtr m_content;

        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestGetAsset, HttpRequestInterface> AppleHttpRequestGetAssetPtr;
    //////////////////////////////////////////////////////////////////////////
}
