#pragma once

#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

#include "Config/Char.h"
#include "Config/Atomic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32ThreadIdentity
        : public ThreadIdentityInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Win32ThreadIdentity );

    public:
        Win32ThreadIdentity();
        ~Win32ThreadIdentity() override;

    public:
        bool initialize( const ConstString & _name, EThreadPriority _priority );
        void finalize();

    public:
        uint64_t getThreadId() const override;

    public:
        ThreadIdentityRunnerInterfacePtr run( const LambdaThreadRunner & _lambda, const DocumentInterfacePtr & _doc ) override;

    public:
        void join() override;

    public:
        bool isCurrentThread() const override;

    public:
        void main();

    protected:
        ConstString m_name;
        EThreadPriority m_priority;

        ThreadIdentityRunnerInterfacePtr m_runner;

        HANDLE m_thread;
        DWORD m_threadId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadIdentity, ThreadIdentityInterface> Win32ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
