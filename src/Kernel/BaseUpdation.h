#pragma once

#include "Interface/UpdationInterface.h"

#include "Kernel/UpdateMode.h"

namespace Mengine
{
    class BaseUpdation
        : public UpdationInterface
    {
    public:
        BaseUpdation();
        ~BaseUpdation() override;

    public:
        bool activate( EUpdateMode _mode, uint32_t _deep ) override final;
        void deactivate() override final;
        void replace( uint32_t _deep ) override final;

    protected:
        uint32_t m_updatableProxyId;
    };
}