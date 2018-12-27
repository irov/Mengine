#pragma once

#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"

#include "Config/Char.h"
#include "Config/Atomic.h"

namespace Mengine
{
    class Win32ThreadIdentity
        : public ThreadIdentityInterface
        , public Factorable
    {
    public:
        Win32ThreadIdentity();
        ~Win32ThreadIdentity() override;

    public:
        bool initialize( const ThreadMutexInterfacePtr & _mutex, int32_t _priority, const Char * _doc, const Char * _file, uint32_t _line );

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

        AtomicBool m_complete;
        AtomicBool m_exit;

#ifndef NDEBUG
        const Char * m_file;
        uint32_t m_line;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadIdentity> Win32ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
