#pragma once

#include "Factory.h"

#include "Interface/ThreadSystemInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FactoryWithMutex
        : public Factory
    {
    public:
        FactoryWithMutex( const char * _name );
        ~FactoryWithMutex() override;

    public:
        void setMutex( const ThreadMutexInterfacePtr & _mutex );

    public:
        FactorablePointer createObject() override;
        void destroyObject( Factorable * _object ) override;

    protected:
        ThreadMutexInterfacePtr m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FactoryWithMutex> FactoryWithMutexPtr;
    //////////////////////////////////////////////////////////////////////////
}
