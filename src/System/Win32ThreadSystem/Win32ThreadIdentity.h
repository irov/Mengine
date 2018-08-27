#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "WIN32/WindowsIncluder.h"

#include "Kernel/ServantBase.h"

#include "Config/Atomic.h"

namespace Mengine
{
    class Win32ThreadIdentity
        : public ServantBase<ThreadIdentityInterface>
    {
    public:
        Win32ThreadIdentity();
        ~Win32ThreadIdentity() override;

    public:
        bool initialize( const ThreadMutexInterfacePtr & _mutex, int _priority, const Char * _file, uint32_t _line );

    public:
        void main();

    public:
        bool processTask( ThreadTaskInterface * _task ) override;
        bool completeTask() override;

    public:
        void join() override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        HANDLE m_handle;

        ThreadTaskInterface * m_task;

        Atomic<bool> m_complete;
        Atomic<bool> m_exit;

#ifndef NDEBUG
        const Char * m_file;
        uint32_t m_line;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadIdentity> Win32ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
