#pragma once

#include "DevToDebugInterface.h"

#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/JSON.h"
#include "Kernel/Identity.h"
#include "Kernel/Hashtable.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Kernel/Scriptable.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidget
        : public DevToDebugWidgetInterface
#ifdef MENGINE_USE_SCRIPT_SERVICE
        , public Scriptable
#endif
    {
    public:
        DevToDebugWidget();
        ~DevToDebugWidget() override;

    public:
        void setId( const ConstString & _id ) override;
        const ConstString & getId() const override;

    public:
        void setBaseProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property ) override;
        const DevToDebugPropertyInterfacePtr & getBaseProperty( const ConstString & _name ) const override;

    public:
        void setDataProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property ) override;
        const DevToDebugPropertyInterfacePtr & getDataProperty( const ConstString & _name ) const override;

    public:
        void syncProperties() override;

    protected:
        virtual void _syncPropertis();

    public:
        bool fillJson( jpp::object & _jwidget, bool _force );

    protected:
        void fillTypeJson( jpp::object & _jdata );
        virtual const Char * getTypeJson() const = 0;

    protected:
        bool fillDataJson( jpp::object & _jdata, bool _force );
        virtual bool _fillDataJson( jpp::object & _jdata, bool _force );

    public:
        virtual void process( const jpp::object & _jdata, const ThreadMutexInterfacePtr & _mutex, VectorDevToDebugWidgetCommands * const _commands ) = 0;

    protected:
        ConstString m_id;

        typedef Hashtable<ConstString, DevToDebugPropertyInterfacePtr> HashtableBaseProperties;
        HashtableBaseProperties m_baseProperties;

        typedef Hashtable<ConstString, DevToDebugPropertyInterfacePtr> HashtableDataProperties;
        HashtableDataProperties m_dataProperties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidget, DevToDebugWidgetInterface> DevToDebugWidgetPtr;
    //////////////////////////////////////////////////////////////////////////
}