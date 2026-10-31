#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class EmscriptenThreadSystem
        : public ServiceBase<ThreadSystemInterface>
    {
    public:
        EmscriptenThreadSystem();
        ~EmscriptenThreadSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool isThreadSupported() const override;

    public:
        ThreadIdentityInterfacePtr createThreadIdentity( const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc ) override;
        ThreadProcessorInterfacePtr createThreadProcessor( const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc ) override;
        ThreadMutexInterfacePtr createMutex( const DocumentInterfacePtr & _doc ) override;
        ThreadSharedMutexInterfacePtr createSharedMutex( const DocumentInterfacePtr & _doc ) override;
        ThreadConditionVariableInterfacePtr createConditionVariable( const DocumentInterfacePtr & _doc ) override;

    public:
        void sleep( uint64_t _ms ) override;

    public:
        ThreadId getCurrentThreadId() const override;
    };
}
