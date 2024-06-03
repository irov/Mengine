#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/BaseHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32HttpRequest
        : public BaseHttpRequest
    {
    public:
        Win32HttpRequest();
        ~Win32HttpRequest() override;

    public:
        void setHInternet( HINTERNET _hInternet );
        HINTERNET getHInternet() const;

    protected:
        void _onThreadTaskPreparation() override;
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;

    protected:
        virtual bool _onHttp() = 0;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        HINTERNET m_hInternet;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32HttpRequest, HttpRequestInterface> Win32HttpRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}
