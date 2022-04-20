#pragma once

#include "DevToDebugWidget.h"

#include "Kernel/String.h"
#include "Kernel/Unknowable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetCheckbox
        : public DevToDebugWidget
        , public UnknownDevToDebugWidgetCheckboxInterface
    {
        DECLARE_FACTORABLE( DevToDebugWidgetCheckbox );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugWidgetCheckbox();
        ~DevToDebugWidgetCheckbox() override;

    public:
        void setClickEvent( const LambdaClickEvent & _clickEvent ) override;

    public:
        void setDataProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property ) override;
        const DevToDebugPropertyInterfacePtr & getDataProperty( const ConstString & _name ) const override;

    protected:
        void _fillTypeJson( jpp::object & _jdata ) override;
        bool _fillDataJson( jpp::object & _jdata, bool _force ) override;

    protected:
        void process( const jpp::object & _data ) override;

    protected:
        LambdaClickEvent m_clickEvent;

        typedef Hashtable<ConstString, DevToDebugPropertyInterfacePtr> HashtableDataProperties;
        HashtableDataProperties m_dataProperties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetCheckbox, DevToDebugWidgetInterface> DevToDebugWidgetCheckboxPtr;
    //////////////////////////////////////////////////////////////////////////
}