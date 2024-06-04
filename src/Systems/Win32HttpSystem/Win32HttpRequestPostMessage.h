#pragma once

#include "Win32HttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32HttpRequestPostMessage
        : public Win32HttpRequest
    {
        DECLARE_FACTORABLE( Win32HttpRequestPostMessage );

    public:
        Win32HttpRequestPostMessage();
        ~Win32HttpRequestPostMessage() override;

    public:
        void setPostProperties( const HttpRequestPostProperties & _properties );
        const HttpRequestPostProperties & getPostProperties() const;

    protected:
        bool _onHttp() override;

    protected:
        HttpRequestPostProperties m_properties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32HttpRequestPostMessage, ThreadTaskInterface> Win32HttpRequestPostMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}
