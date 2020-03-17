#pragma once

#include "GOAPInterface.h"

#include "Kernel/ServiceBase.h"

#include "GOAP/GOAP.h"

namespace Mengine
{
    class GOAPService
        : public ServiceBase<GOAPServiceInterface>
    {
    public:
        GOAPService();
        ~GOAPService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        GOAP::SourceInterfacePtr makeSource() override;
        GOAP::ChainInterfacePtr makeChain( const GOAP::SourceInterfacePtr & _source, const Char * _file, uint32_t _line ) override;

    protected:
        GOAP::SemaphoreInterfacePtr makeSemaphore( int32_t _value ) override;

    protected:
        GOAP::Allocator * m_allocator;

        GOAP::KernelInterfacePtr m_kernel;
    };
}