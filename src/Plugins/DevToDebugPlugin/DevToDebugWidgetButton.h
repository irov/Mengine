#pragma once

#include "DevToDebugWidget.h"

#include "Kernel/String.h"
#include "Kernel/Unknowable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetButton
        : public DevToDebugWidget
        , public UnknownDevToDebugWidgetButtonInterface
    {
        DECLARE_FACTORABLE( DevToDebugWidgetButton );
        DECLARE_UNKNOWABLE();

    public:
        DevToDebugWidgetButton();
        ~DevToDebugWidgetButton() override;

    public:
        void setClickEvent( const LambdaClickEvent & _clickEvent ) override;

    public:
        void setDataProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property ) override;
        const DevToDebugPropertyInterfacePtr & getDataProperty( const ConstString & _name ) const override;

    protected:
        void _syncPropertis() override;

    protected:
        void _fillTypeJson( jpp::object & _jdata ) override;
        bool _fillDataJson( jpp::object & _jdata, bool _force ) override;

    protected:
        void process( const jpp::object & _data, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands ) override;

    protected:
        LambdaClickEvent m_clickEvent;

        typedef Hashtable<ConstString, DevToDebugPropertyInterfacePtr> HashtableDataProperties;
        HashtableDataProperties m_dataProperties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetButton, DevToDebugWidgetInterface> DevToDebugWidgetButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}