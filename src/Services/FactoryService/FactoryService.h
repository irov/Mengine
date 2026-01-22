#pragma once

#include "Interface/FactoryServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"
#include "Kernel/String.h"

#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
    #ifndef MENGINE_NODELEAKDETECTOR_HASHSIZE
    #define MENGINE_NODELEAKDETECTOR_HASHSIZE 16
    #endif
#endif

namespace Mengine
{
    class FactoryService
        : public ServiceBase<FactoryServiceInterface>
    {
        DECLARE_FACTORABLE( FactoryService );

    public:
        FactoryService();
        ~FactoryService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void registerFactory( const FactoryInterface * _factory ) override;
        void unregisterFactory( const FactoryInterface * _factory ) override;

    public:
        void foreachFactories( const LambdaFactory & _lambda ) override;

#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
    protected:
        void debugFactoryIncrefGeneration() override;
        uint32_t debugFactoryGetGeneration() const override;

    protected:
        void debugFactoryCreateObject( const FactoryInterface * _factory, const Factorable * _factorable ) override;
        void debugFactoryDestroyObject( const FactoryInterface * _factory, const Factorable * _factorable ) override;

    protected:
        void debugFactoryForeachLeakObjects( uint32_t _generation, const LambdaFactoryLeaks & _leaks ) const override;
#endif

    protected:
        struct FactoryDesc
        {
            const FactoryInterface * factory;

            //ToDo
        };

        typedef Vector<FactoryDesc> VectorFactories;
        VectorFactories m_factories;

#if defined(MENGINE_DEBUG_FACTORY_ENABLE)
        ThreadMutexInterfacePtr m_mutex;

        uint32_t m_generation;
        bool m_memleakDetection;
        String m_memleakLogFileName;

        struct ObjectLeakDesc
        {
            uint32_t generation;
            const FactoryInterface * factory;
            const Factorable * factorable;
        };

        typedef Vector<ObjectLeakDesc> VectorObjectLeakDesc;
        VectorObjectLeakDesc m_objectLeakDescs[MENGINE_NODELEAKDETECTOR_HASHSIZE];
#endif
    };
}