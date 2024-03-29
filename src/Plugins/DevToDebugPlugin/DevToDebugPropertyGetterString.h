#pragma once

#include "DevToDebugProperty.h"

#include "Interface/ThreadMutexInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugPropertyGetterString
        : public DevToDebugProperty
        , public UnknownDevToDebugPropertyGetterStringInterface
    {
        DECLARE_FACTORABLE( DevToDebugPropertyGetterString );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugPropertyGetterString();
        ~DevToDebugPropertyGetterString() override;

    public:
        void setGetter( const LambdaGetterValue & _getter ) override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void sync() override;

    public:
        bool fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        LambdaGetterValue m_getter;
        mutable String m_cacheAux;
        mutable String m_cache;
        mutable String m_test;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyGetterString, DevToDebugPropertyInterface> DevToDebugPropertyGetterStringPtr;
    //////////////////////////////////////////////////////////////////////////
}