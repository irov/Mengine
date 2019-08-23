#pragma once

#include "Interface/SchedulerInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DelaySchedulePipe
        : public SchedulerPipeInterface
        , public Factorable
    {
    public:
        DelaySchedulePipe();
        ~DelaySchedulePipe() override;

    public:
        void initialize( float _delay );

    protected:
        float onSchedulerPipe( uint32_t _id, uint32_t _index ) override;

    protected:
        float m_delay;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DelaySchedulePipe> DelaySchedulePipePtr;
    //////////////////////////////////////////////////////////////////////////
}