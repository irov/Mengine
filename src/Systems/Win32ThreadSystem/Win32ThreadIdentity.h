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
        bool initialize( const ThreadDescription & _description, EThreadPriority _priority );
        void finalize();

    public:
        EThreadPriority getPriority() const override;
        const ThreadDescription & getDescription() const override;

    public:
        uint64_t getThreadId() const override;

    public:
        ThreadIdentityRunnerInterfacePtr run( const LambdaThreadRunner & _lambda, uint32_t _sleep, const DocumentInterfacePtr & _doc ) override;

    public:
        const ThreadIdentityRunnerInterfacePtr & getRunner() const override;

    public:
        void join( bool _cancel ) override;

    public:
        bool isCurrentThread() const override;

    public:
        void main();

    protected:
        ThreadDescription m_description;
        EThreadPriority m_priority;

        ThreadIdentityRunnerInterfacePtr m_runner;

        HANDLE m_thread;
        DWORD m_threadId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadIdentity, ThreadIdentityInterface> Win32ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
