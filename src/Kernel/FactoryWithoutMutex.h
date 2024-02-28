#pragma once

#include "Kernel/FactoryBase.h"
#include "Kernel/ThreadGuard.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FactoryWithoutMutex
        : public FactoryBase
    {
    public:
        FactoryWithoutMutex();
        ~FactoryWithoutMutex() override;

    public:
        FactorablePointer createObject( const DocumentInterfacePtr & _doc ) override;
        void destroyObject( Factorable * _object ) override;

    public:
        void setMutex( const ThreadMutexInterfacePtr & _mutex ) override;
        const ThreadMutexInterfacePtr & getMutex() const override;

    protected:
        MENGINE_THREAD_GUARD_INIT( FactoryWithoutMutex );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FactoryWithoutMutex, FactoryInterface> FactoryWithoutMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
