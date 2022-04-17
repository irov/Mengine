#pragma once

#include "DevToDebugProperty.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugPropertyGetterBoolean
        : public DevToDebugProperty
        , public UnknownDevToDebugPropertyGetterBooleanInterface
    {
        DECLARE_FACTORABLE( DevToDebugPropertyGetterBoolean );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugPropertyGetterBoolean();
        ~DevToDebugPropertyGetterBoolean();

    public:
        typedef Lambda<void( bool * const )> LambdaGetterValue;
        void setGetter( const LambdaGetterValue & _getter ) override;

    public:
        bool fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const override;

    protected:
        LambdaGetterValue m_getter;
        mutable bool m_test;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyGetterBoolean, DevToDebugPropertyInterface> DevToDebugPropertyGetterBooleanPtr;
    //////////////////////////////////////////////////////////////////////////
}