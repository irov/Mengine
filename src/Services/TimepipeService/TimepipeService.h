#pragma once

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Vector.h"

namespace Mengine
{
    class TimepipeService
        : public ServiceBase<TimepipeServiceInterface>
    {
    public:
        TimepipeService();
        ~TimepipeService() override;

    public:
        uint32_t addTime( const LambdaTimepipe & _timepipe ) override;
        void removeTime( uint32_t _id ) override;

    public:
        void tick( const UpdateContext * _context ) override;

    protected:
        uint32_t m_enumerator;

        struct TimepipeDesc
        {
            uint32_t id;
            LambdaTimepipe timepipe;
        };

        typedef Vector<TimepipeDesc> VectorTimepipe;
        VectorTimepipe m_timepipeAdd;
        VectorTimepipe m_timepipe;
    };
};
