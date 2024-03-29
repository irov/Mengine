#pragma once

#include "DevToDebugProperty.h"

#include "Interface/ThreadMutexInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugPropertyGetterColor
        : public DevToDebugProperty
        , public UnknownDevToDebugPropertyGetterColorInterface
    {
        DECLARE_FACTORABLE( DevToDebugPropertyGetterColor );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugPropertyGetterColor();
        ~DevToDebugPropertyGetterColor() override;

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
        mutable Color m_cache;
        mutable Color m_test;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyGetterColor, DevToDebugPropertyInterface> DevToDebugPropertyGetterColorPtr;
    //////////////////////////////////////////////////////////////////////////
}