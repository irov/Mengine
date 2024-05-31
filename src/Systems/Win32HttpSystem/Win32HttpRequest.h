#pragma once

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
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32HttpRequest, HttpRequestInterface> Win32HttpRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}
