#pragma once

#include "Interface/ProfilerSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "OptickProfilerDescription.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/Atomic.h"

#include "optick.h"

namespace Mengine
{
    class OptickProfilerSystem
        : public ServiceBase<ProfilerSystemInterface>
    {
        DECLARE_FACTORABLE( OptickProfilerSystem );

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
        ProfilerInterfacePtr addFrame( const ProfilerDescriptionInterface * _description ) override;
        ProfilerInterfacePtr addCategory( const ProfilerDescriptionInterface * _description ) override;

    public:
        const ProfilerDescriptionInterface * createDescription( const Char * _name, const Char * _file, int32_t _line ) override;

    protected:
        AtomicBool m_process;

        ThreadMutexInterfacePtr m_mutex;

        FactoryInterfacePtr m_factoryThreadProfilers;
        FactoryInterfacePtr m_factoryFrameProfilers;
        FactoryInterfacePtr m_factoryCategoryProfilers;
        FactoryInterfacePtr m_factoryDescriptions;

        typedef Vector<OptickProfilerDescriptionPtr> VectorProfilerDescriptions;
        VectorProfilerDescriptions m_descriptions;
    };
}
