#pragma once

#include "Config/Typedef.h"

#include "Interface/Interface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/FactoryInterface.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_DEBUG_FACTORY
#   if defined(MENGINE_DEBUG)
#       define MENGINE_DEBUG_FACTORY 1
#   else
#       define MENGINE_DEBUG_FACTORY 0
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_DEBUG_FACTORY == 1
#   define MENGINE_DEBUG_FACTORY_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{    
    class FactoryServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FactoryService" )

    public:
        virtual void registerFactory( const FactoryInterface * _factory ) = 0;
        virtual void unregisterFactory( const FactoryInterface * _factory ) = 0;

    public:
        typedef Lambda<void( const FactoryInterface * )> LambdaFactory;
        virtual void foreachFactories( const LambdaFactory & _lambda ) = 0;

#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
    public:
        virtual void debugFactoryIncrefGeneration() = 0;
        virtual uint32_t debugFactoryGetGeneration() const = 0;

    public:
        virtual void debugFactoryCreateObject( const FactoryInterface * _factory, const Factorable * _factorable ) = 0;
        virtual void debugFactoryDestroyObject( const FactoryInterface * _factory, const Factorable * _factorable ) = 0;

    public:
        typedef Lambda<void( const FactoryInterface * _factory, const Factorable * )> LambdaFactoryLeaks;
        virtual void debugFactoryForeachLeakObjects( uint32_t _generation, const LambdaFactoryLeaks & _leaks ) const = 0;
#endif
    };
}
//////////////////////////////////////////////////////////////////////////
#define FACTORY_SERVICE()\
    ((Mengine::FactoryServiceInterface *)SERVICE_GET(Mengine::FactoryServiceInterface))
//////////////////////////////////////////////////////////////////////////
