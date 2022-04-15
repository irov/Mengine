#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"
#include "Interface/UnknownInterface.h"

#include "Kernel/Unknowable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetInterface
        : public ServantInterface
        , public Unknowable
    {
    public:
        virtual void setId( const ConstString & _id ) = 0;
        virtual const ConstString & getId() const = 0;

        virtual void setHide( bool _hide ) = 0;
        virtual bool getHide() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetInterface> DevToDebugWidgetInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugWidgetTextInterface
        : public UnknownInterface
    {
    public:
        virtual void setConstText( const String & _text ) = 0;

        typedef Lambda<void( String * const )> LambdaGetterText;
        virtual void setGetterText( const LambdaGetterText & _getter ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugWidgetTextInterface, UnknownInterface> UnknownDevToDebugWidgetTextInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugWidgetButtonInterface
        : public UnknownInterface
    {
    public:
        virtual void setTitle( const String & _title ) = 0;
        virtual const String & getTitle() const = 0;

    public:
        typedef Lambda<void()> LambdaClickEvent;
        virtual void setClickEvent( const LambdaClickEvent & _clickEvent ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugWidgetButtonInterface, UnknownInterface> UnknownDevToDebugWidgetButtonInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugTabInterface
        : public ServantInterface
    {
    public:
        virtual void addWidget( const DevToDebugWidgetInterfacePtr & _widget ) = 0;
        virtual const DevToDebugWidgetInterfacePtr & findWidget( const ConstString & _id ) const = 0;

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