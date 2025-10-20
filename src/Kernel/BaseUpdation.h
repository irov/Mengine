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
        MENGINE_INLINE EUpdateMode getUpdationMode() const final;
        MENGINE_INLINE uint32_t getUpdationDeep() const final;

    private:
        EUpdateMode m_mode;
        uint32_t m_deep;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE EUpdateMode BaseUpdation::getUpdationMode() const
    {
        return m_mode;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t BaseUpdation::getUpdationDeep() const
    {
        return m_deep;
    }
    //////////////////////////////////////////////////////////////////////////
}