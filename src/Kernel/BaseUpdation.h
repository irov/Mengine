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
        void update( const UpdateContext * _context ) override;

    protected:
        virtual void _update( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}