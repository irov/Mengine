#pragma once

#include "DevToDebugProperty.h"

#include "Kernel/Color.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugPropertyConstColor
        : public DevToDebugProperty
        , public UnknownDevToDebugPropertyConstColorInterface
    {
        DECLARE_FACTORABLE( DevToDebugPropertyConstColor );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugPropertyConstColor();
        ~DevToDebugPropertyConstColor();

    public:
        void setValue( const Color & _value ) override;
        const Color & getValue() const override;

    public:
        bool fillPropertyJson( const ConstString & _name, jpp::object & _jwidget, bool _force ) const override;

    protected:
        mutable bool m_invalidate;
        Color m_value;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyConstColor, DevToDebugPropertyInterface> DevToDebugPropertyConstColorPtr;
    //////////////////////////////////////////////////////////////////////////
}