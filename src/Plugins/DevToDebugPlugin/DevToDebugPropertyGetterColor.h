#pragma once

#include "DevToDebugProperty.h"

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
        ~DevToDebugPropertyGetterColor();

    public:
        typedef Lambda<void( Color * const )> LambdaGetterValue;
        void setGetter( const LambdaGetterValue & _getter ) override;

    public:
        bool fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const override;

    protected:
        LambdaGetterValue m_getter;
        mutable Color m_test;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyGetterColor, DevToDebugPropertyInterface> DevToDebugPropertyGetterColorPtr;
    //////////////////////////////////////////////////////////////////////////
}