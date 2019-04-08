#pragma once

#include "NodeLeakDetectorServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Observable.h"

#include "Config/Vector.h"
#include "Config/String.h"

#ifndef MENGINE_NODELEAKDETECTOR_HASHSIZE
#define MENGINE_NODELEAKDETECTOR_HASHSIZE 1021
#endif

namespace Mengine
{
    class NodeLeakDetectorService
        : public ServiceBase<NodeLeakDetectorServiceInterface>
        , public Observable
    {
    public:
        NodeLeakDetectorService();
        ~NodeLeakDetectorService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void increfGeneration() override;
        uint32_t getGeneration() const override;
        void getNodeLeak( uint32_t _generation, const LambdaNodeLeaks & _leaks ) const override;

    protected:
        void notifyDebugCreateObject( Factory * _factory, Factorable * _factorable, const Char * _doc );
        void notifyDebugDestroyObject( Factory * _factory, Factorable * _factorable );

    protected:
        uint32_t m_generation;

        struct NodeLeakDesc
        {
            uint32_t generation;
            const Factory * factory;
            const Factorable * factorable;
            String doc;
        };

        typedef Vector<NodeLeakDesc> VectorNodeLeakDesc;
        VectorNodeLeakDesc m_nodeLeakDescs[MENGINE_NODELEAKDETECTOR_HASHSIZE];
    };
}
//////////////////////////////////////////////////////////////////////////
#   define NODELEAKDETECTOR_SERVICE()\
	((NodeLeakDetectorServiceInterface *)SERVICE_GET(Mengine::NodeLeakDetectorServiceInterface))
//////////////////////////////////////////////////////////////////////////