#pragma once

#include "Kernel/FactoryBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FactoryWithMutex
        : public FactoryBase
    {
    public:
        FactoryWithMutex();
        ~FactoryWithMutex() override;

    public:
        FactorablePointer createObject( const DocumentInterfacePtr & _doc ) override;
        void destroyObject( Factorable * _object ) override;

    public:
        void setMutex( const ThreadMutexInterfacePtr & _mutex ) override;
        const ThreadMutexInterfacePtr & getMutex() const override;

    protected:
        ThreadMutexInterfacePtr m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FactoryWithMutex, FactoryInterface> FactoryWithMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
