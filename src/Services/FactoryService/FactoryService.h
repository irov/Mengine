#pragma once

#include "Interface/FactoryServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"
#include "Kernel/String.h"

#ifndef MENGINE_NODELEAKDETECTOR_HASHSIZE
#define MENGINE_NODELEAKDETECTOR_HASHSIZE 16
#endif

namespace Mengine
{
    class FactoryService
        : public ServiceBase<FactoryServiceInterface>
    {
    public:
        FactoryService();
        ~FactoryService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void registerFactory( const Factory * _factory ) override;
        void unregisterFactory( const Factory * _factory ) override;

    public:
        void foreachFactories( const LambdaFactory & _lambda ) override;

    protected:
        void debugFactoryCreateObject( const Factory * _factory, const Factorable * _factorable, const DocumentPtr & _doc ) override;
        void debugFactoryDestroyObject( const Factory * _factory, const Factorable * _factorable ) override;

    protected:
        void increfFactoryGeneration() override;
        uint32_t getFactoryGeneration() const override;
        void foreachFactoryLeakObjects( uint32_t _generation, const LambdaFactoryLeaks & _leaks ) const override;

    protected:
        struct FactoryDesc
        {
            const Factory * factory;

#ifdef MENGINE_DEBUG
            String factory_name;
#endif
        };

        typedef Vector<FactoryDesc> VectorFactories;
        VectorFactories m_factories;

#ifdef MENGINE_DEBUG
        ThreadMutexInterfacePtr m_mutex;

        uint32_t m_generation;
        bool m_memleakDetection;
        String m_memleakLogFileName;

        struct ObjectLeakDesc
        {
            uint32_t generation;
            const Factory * factory;
            const Factorable * factorable;
            String factory_name;
            DocumentPtr factorable_doc;
        };

        typedef Vector<ObjectLeakDesc> VectorObjectLeakDesc;
        VectorObjectLeakDesc m_objectLeakDescs[MENGINE_NODELEAKDETECTOR_HASHSIZE];
#endif
    };
}