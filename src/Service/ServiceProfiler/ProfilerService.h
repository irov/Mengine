#pragma once

#include "Interface/ProfilerInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class ProfilerService
        : public ServiceBase<ProfilerServiceInterface>
    {
    public:
        ProfilerService();
        ~ProfilerService() override;

    protected:
        bool _initializeService() override;
		void _finalizeService() override;

    protected:
        void memoryBegin() override;
        size_t memoryEnd() override;

        size_t getMemoryUsage() const override;

    protected:
        void * m_checkpoint;
        size_t m_beginMemoryUsage;
    };
}