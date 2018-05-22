#pragma once

#include "Interface/ScheduleManagerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DelaySchedulePipe
        : public SchedulePipeInterface
    {
    public:
        DelaySchedulePipe();
        ~DelaySchedulePipe() override;

    public:
        void initialize( float _delay );

    protected:
        float onSchedulePipe( uint32_t _id, uint32_t _index ) override;

    protected:
        float m_delay;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DelaySchedulePipe> DelaySchedulePipePtr;
    //////////////////////////////////////////////////////////////////////////
}