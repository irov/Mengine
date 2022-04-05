#pragma once

#include "Kernel/Factory.h"

#include "Interface/ThreadMutexInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FactoryWithMutex
        : public Factory
    {
    public:
        FactoryWithMutex();
        ~FactoryWithMutex() override;

    public:
        void setMutex( const ThreadMutexInterfacePtr & _mutex );
        const ThreadMutexInterfacePtr & getMutex() const;

    public:
        FactorablePointer createObject( const DocumentPtr & _doc ) override;
        FactorableVirtualInheritancePointer createVirtualInheritanceObject( const DocumentPtr & _doc ) override;
        void destroyObject( Factorable * _object ) override;

    protected:
        ThreadMutexInterfacePtr m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FactoryWithMutex> FactoryWithMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
