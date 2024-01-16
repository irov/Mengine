#pragma once

#include "Interface/TimerServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class TimerService
        : public ServiceBase<TimerServiceInterface>
    {
    public:
        TimerService();
        ~TimerService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        UniqueId addTimer( float _delay, const LambdaTimer & _lambda, const DocumentInterfacePtr & _doc ) override;
        void removeTimer( UniqueId _id ) override;

    public:
        void update( float _time ) override;
        
    protected:
        struct TimerDesc
        {
            UniqueId id;
            float milliseconds;
            float time;
            LambdaTimer lambda;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<TimerDesc> VectorTimers;
        VectorTimers m_timers;
    };
};
