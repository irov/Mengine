#pragma once

#include "DevToDebugProperty.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugPropertyConstBoolean
        : public DevToDebugProperty
        , public UnknownDevToDebugPropertyConstBooleanInterface
    {
        DECLARE_FACTORABLE( DevToDebugPropertyConstBoolean );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugPropertyConstBoolean();
        ~DevToDebugPropertyConstBoolean();

    public:
        void setValue( bool _value ) override;
        bool getValue() const override;

    public:
        bool fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const override;

    protected:
        mutable bool m_invalidate;
        bool m_value;        
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyConstBoolean, DevToDebugPropertyInterface> DevToDebugPropertyConstBooleanPtr;
    //////////////////////////////////////////////////////////////////////////
}