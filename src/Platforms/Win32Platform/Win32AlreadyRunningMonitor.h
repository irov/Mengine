#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Typedef.h"
#include "Kernel/String.h"

#include "Kernel/Factorable.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EAlreadyRunningPolicy
    {
        EARP_NONE,
        EARP_SETFOCUS,
        EARP_SHOWMESSAGE
    };
    //////////////////////////////////////////////////////////////////////////
    class Win32AlreadyRunningMonitor
        : public Factorable
    {
        DECLARE_FACTORABLE( Win32AlreadyRunningMonitor );

    public:
        Win32AlreadyRunningMonitor();
        ~Win32AlreadyRunningMonitor() override;

    public:
        bool initialize( EAlreadyRunningPolicy _policy, const WChar * _windowClassName, const WChar * _projectTitle, bool * const _stop );
        void finalize();

    protected:
        HANDLE m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32AlreadyRunningMonitor> Win32AlreadyRunningMonitorPtr;
    //////////////////////////////////////////////////////////////////////////
}
