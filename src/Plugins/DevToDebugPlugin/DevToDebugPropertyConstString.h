#pragma once

#include "DevToDebugProperty.h"

#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugPropertyConstString
        : public DevToDebugProperty
        , UnknownDevToDebugPropertyConstStringInterface
    {
        DECLARE_FACTORABLE( DevToDebugPropertyConstString );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugPropertyConstString();
        ~DevToDebugPropertyConstString();

    public:
        void setValue( const String & _value ) override;
        const String & getValue() const override;

    public:
        bool fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const override;

    protected:
        mutable bool m_invalidate;
        String m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyConstString, DevToDebugPropertyInterface> DevToDebugPropertyConstStringPtr;
    //////////////////////////////////////////////////////////////////////////
}