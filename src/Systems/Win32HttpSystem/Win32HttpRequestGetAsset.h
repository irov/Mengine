#pragma once

#include "Win32HttpRequest.h"

#include "Interface/OutputStreamInterface.h"
#include "Interface/ContentInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32HttpRequestGetAsset
        : public Win32HttpRequest
    {
        DECLARE_FACTORABLE( Win32HttpRequestGetAsset );

    public:
        Win32HttpRequestGetAsset();
        ~Win32HttpRequestGetAsset() override;

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
        bool _onHttp() override;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        String m_login;
        String m_password;
        ContentInterfacePtr m_content;

        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32HttpRequestGetAsset, HttpRequestInterface> Win32HttpRequestGetAssetPtr;
    //////////////////////////////////////////////////////////////////////////
}
