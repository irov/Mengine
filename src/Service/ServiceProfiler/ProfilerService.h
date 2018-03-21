#pragma once

#include "Interface/ProfilerInterface.h"

#include "Core/ServiceBase.h"

namespace Mengine
{
    class ProfilerService
        : public ServiceBase<ProfilerServiceInterface>
    {
    public:
        ProfilerService();
        ~ProfilerService();

    protected:
        bool _initialize() override;
		void _finalize() override;

    protected:
        void memoryBegin() override;
        size_t memoryEnd() override;

        size_t getMemoryUsage() const override;

    protected:
        void * m_checkpoint;
        size_t m_beginMemoryUsage;
    };
}