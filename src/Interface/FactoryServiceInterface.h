#pragma once

#include "Config/Typedef.h"

#include "Interface/Interface.h"
#include "Interface/ServiceInterface.h"

#include "Kernel/Factory.h"

namespace Mengine
{    
    class FactoryServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FactoryService" )

    public:
        virtual void registerFactory( const Factory * _factory ) = 0;
        virtual void unregisterFactory( const Factory * _factory ) = 0;

    public:
        typedef Lambda<void( const Factory * )> LambdaFactory;
        virtual void foreachFactories( const LambdaFactory & _lambda ) = 0;

    public:
        virtual void increfFactoryGeneration() = 0;
        virtual uint32_t getFactoryGeneration() const = 0;

        typedef Lambda<void( const Factory * _factory, const Factorable *, const Char * _type, const DocumentPtr & _doc )> LambdaFactoryLeaks;
        virtual void visitFactoryLeakObjects( uint32_t _generation, const LambdaFactoryLeaks & _leaks ) const = 0;

    public:
        virtual void debugFactoryCreateObject( const Factory * _factory, const Factorable * _factorable, const DocumentPtr & _doc ) = 0;
        virtual void debugFactoryDestroyObject( const Factory * _factory, const Factorable * _factorable ) = 0;
    };    
}
//////////////////////////////////////////////////////////////////////////
#define FACTORY_SERVICE()\
    ((Mengine::FactoryServiceInterface *)SERVICE_GET(Mengine::FactoryServiceInterface))
//////////////////////////////////////////////////////////////////////////
