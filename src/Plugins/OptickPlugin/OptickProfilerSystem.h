#pragma once

#include "Interface/ProfilerSystemInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "OptickProfilerDescription.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"
#include "Kernel/Vector.h"

#include "Config/Atomic.h"

#include "optick.h"

namespace Mengine
{
    class OptickProfilerSystem
        : public ServiceBase<ProfilerSystemInterface>
    {
    public:
        OptickProfilerSystem();
        ~OptickProfilerSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void beginApplication() override;
        void endApplication() override;

        ProfilerInterfacePtr addThread( const Char * _threadName ) override;
        ProfilerInterfacePtr addFrame( ProfilerDescriptionInterface * _description ) override;
        ProfilerInterfacePtr addCategory( ProfilerDescriptionInterface * _description ) override;

    public:
        ProfilerDescriptionInterface * createDescription( const Char * _name, const Char * _file, uint32_t _line ) override;

    protected:
        AtomicBool m_process;

        ThreadMutexInterfacePtr m_mutex;

        FactoryPtr m_factoryThreadProfilers;
        FactoryPtr m_factoryFrameProfilers;
        FactoryPtr m_factoryCategoryProfilers;
        FactoryPtr m_factoryDescriptions;

        typedef Vector<OptickProfilerDescriptionPtr> VectorProfilerDescriptions;
        VectorProfilerDescriptions m_descriptions;        
    };
}
