#pragma once

#include "DevToDebugProperty.h"

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
        typedef Lambda<void( String * const )> LambdaGetterValue;
        void setGetter( const LambdaGetterValue & _getter ) override;

    public:
        bool fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const override;

    protected:
        LambdaGetterValue m_getter;
        mutable String m_test;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyGetterString, DevToDebugPropertyInterface> DevToDebugPropertyGetterStringPtr;
    //////////////////////////////////////////////////////////////////////////
}