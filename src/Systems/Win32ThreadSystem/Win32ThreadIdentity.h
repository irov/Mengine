#pragma once

#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Document.h"
#include "Kernel/DocumentHelper.h"

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
        bool initialize( const ConstString & _name, EThreadPriority _priority, const DocumentPtr & _doc );
        void finalize();

    public:
        uint64_t getThreadId() const override;

    public:
        ThreadIdentityRunnerInterfacePtr run( const LambdaThreadRunner & _lambda ) override;

    public:
        void join() override;
        void cancel() override;

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

#ifdef MENGINE_DOCUMENT_ENABLE
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32ThreadIdentity, ThreadIdentityInterface> Win32ThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
