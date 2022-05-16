#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"
#include "Interface/UnknownInterface.h"

#include "Kernel/Unknowable.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"
#include "Kernel/Color.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void()> DevToDebugWidgetCommand;
    typedef Vector<DevToDebugWidgetCommand> VectorDevToDebugWidgetCommands;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugPropertyInterface
        : public ServantInterface
        , public Unknowable
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void sync() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugPropertyInterface> DevToDebugPropertyInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugPropertyConstBooleanInterface
        : public UnknownInterface
    {
    public:
        virtual void setValue( bool _value ) = 0;
        virtual bool getValue() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugPropertyConstBooleanInterface> UnknownDevToDebugPropertyConstBooleanInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugPropertyGetterBooleanInterface
        : public UnknownInterface
    {
    public:
        typedef Lambda<void( bool * const )> LambdaGetterValue;
        virtual void setGetter( const LambdaGetterValue & _getter ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugPropertyGetterBooleanInterface> UnknownDevToDebugPropertyGetterBooleanInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugPropertyInitialBooleanInterface
        : public UnknownInterface
    {
    public:
        virtual void setValue( bool _value ) = 0;
        virtual bool getValue() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugPropertyInitialBooleanInterface> UnknownDevToDebugPropertyInitialBooleanInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugPropertyConstStringInterface
        : public UnknownInterface
    {
    public:
        virtual void setValue( const String & _value ) = 0;
        virtual const String & getValue() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugPropertyConstStringInterface> UnknownDevToDebugPropertyConstStringInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugPropertyGetterStringInterface
        : public UnknownInterface
    {
    public:
        typedef Lambda<void( String * const )> LambdaGetterValue;
        virtual void setGetter( const LambdaGetterValue & _getter ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugPropertyGetterStringInterface> UnknownDevToDebugPropertyGetterStringInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugPropertyConstColorInterface
        : public UnknownInterface
    {
    public:
        virtual void setValue( const Color & _value ) = 0;
        virtual const Color & getValue() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugPropertyConstColorInterface> UnknownDevToDebugPropertyConstColorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugPropertyGetterColorInterface
        : public UnknownInterface
    {
    public:
        typedef Lambda<void( Color * const )> LambdaGetterValue;
        virtual void setGetter( const LambdaGetterValue & _getter ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugPropertyGetterColorInterface> UnknownDevToDebugPropertyGetterColorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugWidgetInterface
        : public ServantInterface
        , public Unknowable
    {
    public:
        virtual void setId( const ConstString & _id ) = 0;
        virtual const ConstString & getId() const = 0;

    public:
        virtual void setBaseProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property ) = 0;
        virtual const DevToDebugPropertyInterfacePtr & getBaseProperty( const ConstString & _name ) const = 0;

    public:
        virtual void syncProperties() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DevToDebugWidgetInterface> DevToDebugWidgetInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugWidgetTextInterface
        : public UnknownInterface
    {
    public:
        virtual void setDataProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property ) = 0;
        virtual const DevToDebugPropertyInterfacePtr & getDataProperty( const ConstString & _name ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugWidgetTextInterface> UnknownDevToDebugWidgetTextInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugWidgetCheckboxInterface
        : public UnknownInterface
    {
    public:
        typedef Lambda<void(bool)> LambdaClickEvent;
        virtual void setClickEvent( const LambdaClickEvent & _clickEvent ) = 0;

    public:
        virtual void setDataProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property ) = 0;
        virtual const DevToDebugPropertyInterfacePtr & getDataProperty( const ConstString & _name ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugWidgetCheckboxInterface> UnknownDevToDebugWidgetCheckboxInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownDevToDebugWidgetButtonInterface
        : public UnknownInterface
    {
    public:
        typedef Lambda<void()> LambdaClickEvent;
        virtual void setClickEvent( const LambdaClickEvent & _clickEvent ) = 0;

    public:
        virtual void setDataProperty( const ConstString & _name, const DevToDebugPropertyInterfacePtr & _property ) = 0;
        virtual const DevToDebugPropertyInterfacePtr & getDataProperty( const ConstString & _name ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownDevToDebugWidgetButtonInterface> UnknownDevToDebugWidgetButtonInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugTabInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

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
        virtual const DevToDebugTabInterfacePtr & getTab( const ConstString & _name ) const = 0;
        virtual bool hasTab( const ConstString & _name ) const = 0;
        virtual void removeTab( const ConstString & _name ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DEVTODEBUG_SERVICE()\
    ((Mengine::DevToDebugServiceInterface *)SERVICE_GET(Mengine::DevToDebugServiceInterface))
//////////////////////////////////////////////////////////////////////////