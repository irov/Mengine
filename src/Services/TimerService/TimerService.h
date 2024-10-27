#pragma once

#include "Interface/TimerServiceInterface.h"
#include "Interface/TimepipeInterface.h"

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
        UniqueId addTimer( Timestamp _delay, const LambdaTimer & _lambda, const DocumentInterfacePtr & _doc ) override;
        void removeTimer( UniqueId _id ) override;

    public:
        void update( Timestamp _frameTime ) override;
        
    protected:
        struct TimerDesc
        {
            UniqueId id;
            Timestamp milliseconds;
            int64_t time;
            LambdaTimer lambda;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<TimerDesc> VectorTimers;
        VectorTimers m_timers;
    };
};
