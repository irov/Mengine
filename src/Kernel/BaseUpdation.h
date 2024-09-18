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
        void activate( EUpdateMode _mode, uint32_t _deep ) override final;
        void deactivate() override final;
        void replace( uint32_t _deep ) override final;

    public:
        EUpdateMode getMode() const final;
        EUpdateState getState() const final;
        uint32_t getDeep() const final;

    private:
        EUpdateMode m_mode;
        EUpdateState m_state;
        uint32_t m_deep;
    };
}