#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/Interface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetInterface
        : public Interface
    {
    public:
        virtual void setId( const ConstString & _id ) = 0;
        virtual const ConstString & getId() const = 0;
    };
    typedef IntrusivePtr<DevToDebugWidgetInterface> DevToDebugWidgetInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetTextInterface
        : public DevToDebugWidgetInterface
    {
    public:
        virtual void setConstText( const String & _text ) = 0;

        typedef Lambda<void( String & )> LambdaGetterText;
        virtual void setGetterTitle( const LambdaGetterText & _getter ) = 0;

        virtual const String & calculateText() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetTextInterface, DevToDebugWidgetInterface> DevToDebugWidgetTextInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetButtonInterface
        : public DevToDebugWidgetInterface
    {
    public:
        virtual void setTitle( const String & _title ) = 0;
        virtual const String & getTitle() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetButtonInterface, DevToDebugWidgetInterface> DevToDebugWidgetButtonInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugProviderInterface
        : public Interface
    {
    public:
        virtual void addWidget( const DevToDebugWidgetInterfacePtr & _widget ) = 0;

        typedef Lambda<void( const DevToDebugWidgetInterfacePtr & )> LambdaForeachWidgets;
        virtual void foreachWidgets( const LambdaForeachWidgets & _lambda ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugProviderInterface> DevToDebugProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "DevToDebugService" )

    public:
        virtual void addProvider( const ConstString & _type, const DevToDebugProviderInterfacePtr & _provider ) = 0;
        virtual void removeProvider( const ConstString & _type ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DEVTODEBUG_SERVICE()\
    ((Mengine::DevToDebugServiceInterface *)SERVICE_GET(Mengine::DevToDebugServiceInterface))
//////////////////////////////////////////////////////////////////////////