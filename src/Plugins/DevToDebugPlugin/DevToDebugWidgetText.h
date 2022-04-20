#pragma once

#include "DevToDebugWidget.h"

#include "Kernel/String.h"
#include "Kernel/Unknowable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetText
        : public DevToDebugWidget
        , public UnknownDevToDebugWidgetTextInterface
    {
        DECLARE_FACTORABLE( DevToDebugWidgetText );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugWidgetText();
        ~DevToDebugWidgetText() override;

    public:
        void setDataProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property ) override;
        const DevToDebugPropertyInterfacePtr & getDataProperty( const ConstString & _name ) const override;

    protected:
        void _fillTypeJson( jpp::object & _jdata ) override;
        bool _fillDataJson( jpp::object & _jdata, bool _force ) override;

    protected:
        void process( const jpp::object & _data ) override;

    protected:
        typedef Hashtable<ConstString, DevToDebugPropertyInterfacePtr> HashtableDataProperties;
        HashtableDataProperties m_dataProperties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetText, DevToDebugWidgetInterface> DevToDebugWidgetTextPtr;
    //////////////////////////////////////////////////////////////////////////
}