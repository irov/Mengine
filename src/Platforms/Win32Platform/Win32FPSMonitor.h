#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32FPSMonitor
        : public Factorable
    {
    public:
        Win32FPSMonitor();
        ~Win32FPSMonitor() override;

    public:
        bool initialize();
        void finalize();

    public:
        void setActive( bool _active );
        void setFrameTime( float _frameTime );

    public:
        void monitor();

    public:
        DWORD WINAPI threadFrameSignal();

    protected:
        bool m_running;
        bool m_active;

        float m_frameTime;

        DWORD  m_threadId;

        HANDLE m_hFrameSignalEvent;
        HANDLE m_hDestroySignalEvent;
        HANDLE m_hFrameSignalThread;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32FPSMonitor> Win32FPSMonitorPtr;
    //////////////////////////////////////////////////////////////////////////
}
