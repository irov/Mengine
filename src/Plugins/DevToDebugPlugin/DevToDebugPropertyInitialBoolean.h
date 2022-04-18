#pragma once

#include "DevToDebugProperty.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugPropertyInitialBoolean
        : public DevToDebugProperty
        , public UnknownDevToDebugPropertyInitialBooleanInterface
    {
        DECLARE_FACTORABLE( DevToDebugPropertyInitialBoolean );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugPropertyInitialBoolean();
        ~DevToDebugPropertyInitialBoolean() override;

    public:
        void setValue( bool _value ) override;
        bool getValue() const override;

    public:
        bool fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const override;

    protected:
        bool m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyInitialBoolean, DevToDebugPropertyInterface> DevToDebugPropertyInitialBooleanPtr;
    //////////////////////////////////////////////////////////////////////////
}