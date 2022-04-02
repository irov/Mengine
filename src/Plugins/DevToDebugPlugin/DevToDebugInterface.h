#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/Interface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct DevToDebugWidgetTextData
    {
        String text;
    };
    //////////////////////////////////////////////////////////////////////////
    struct DevToDebugWidgetButtonData
    {
        String title;
    };
    //////////////////////////////////////////////////////////////////////////
    class DevToDebugProviderInterface
        : public Interface
    {
    public:
        virtual void addWidgetText( const ConstString & _id, const DevToDebugWidgetTextData & _data ) = 0;
        virtual void addWidgetButton( const ConstString & _id, const DevToDebugWidgetButtonData & _data ) = 0;
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