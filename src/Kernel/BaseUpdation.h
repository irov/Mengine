#pragma once

#include "Interface/UpdationInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseUpdation
        : public UpdationInterface
    {
    public:
        BaseUpdation();
        ~BaseUpdation() override;

    public:
        void activate( uint32_t _mode, uint32_t _deep ) override;
        void deactivate() override;
        void replace( uint32_t _deep ) override;

    public:
        void update( const UpdateContext * _context ) override;

    protected:
        virtual void _update( const UpdateContext * _context ) = 0;

    protected:
        uint32_t m_updatableProxyId;
    };
    //////////////////////////////////////////////////////////////////////////
}