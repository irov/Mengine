#   pragma once

#   include "Interface/ThreadSystemInterface.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
    class Win32ThreadMutex
        : public ThreadMutexInterface
    {
    public:
        Win32ThreadMutex();
        
    public:
        void initialize( HANDLE _mutex );

    protected:
        bool lock() override;
        void unlock() override;

    protected:
        HANDLE m_mutex;
    };
}