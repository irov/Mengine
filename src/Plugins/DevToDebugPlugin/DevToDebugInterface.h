#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/Interface.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetInterface
        : public Interface
    {
    public:
        virtual void setId( const ConstString & _id ) = 0;
        virtual const ConstString & getId() const = 0;

        virtual void setHide( bool _hide ) = 0;
        virtual bool getHide() const = 0;

    public:
        virtual void fillJson( jpp::object & _jwidget ) = 0;
    };
    typedef IntrusivePtr<DevToDebugWidgetInterface> DevToDebugWidgetInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetTextInterface
        : virtual public DevToDebugWidgetInterface
    {
    public:
        virtual void setConstText( const String & _text ) = 0;

        typedef Lambda<void( String & )> LambdaGetterText;
        virtual void setGetterTitle( const LambdaGetterText & _getter ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetTextInterface, DevToDebugWidgetInterface> DevToDebugWidgetTextInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetButtonInterface
        : virtual public DevToDebugWidgetInterface
    {
    public:
        virtual void setTitle( const String & _title ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetButtonInterface, DevToDebugWidgetInterface> DevToDebugWidgetButtonInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugTabInterface
        : public Interface
    {
    public:
        virtual void addWidget( const DevToDebugWidgetInterfacePtr & _widget ) = 0;

        typedef Lambda<void( const DevToDebugWidgetInterfacePtr & )> LambdaForeachWidgets;
        virtual void foreachWidgets( const LambdaForeachWidgets & _lambda ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugTabInterface> DevToDebugTabInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "DevToDebugService" )

    public:
        virtual void addTab( const ConstString & _name, const DevToDebugTabInterfacePtr & _tab ) = 0;
        virtual void removeTab( const ConstString & _name ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DEVTODEBUG_SERVICE()\
    ((Mengine::DevToDebugServiceInterface *)SERVICE_GET(Mengine::DevToDebugServiceInterface))
//////////////////////////////////////////////////////////////////////////