#pragma once

#include "Config/Typedef.h"

#include "Interface/Interface.h"
#include "Interface/ServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Factory;
    //////////////////////////////////////////////////////////////////////////
    class VisitorFactoryInterface
        : public Interface
    {
    public:
        virtual void visit( const Factory * _factory ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VisitorFactoryInterface> VisitorFactoryServicePtr;
    //////////////////////////////////////////////////////////////////////////
    class FactoryServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FactoryService" )

    public:
        virtual void registerFactory( const Factory * _factory ) = 0;
        virtual void unregisterFactory( const Factory * _factory ) = 0;

    public:
        virtual void visitFactories( const VisitorFactoryServicePtr & _visitor ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define FACTORY_SERVICE()\
	((Mengine::FactoryServiceInterface *)SERVICE_GET(Mengine::FactoryServiceInterface))
//////////////////////////////////////////////////////////////////////////
