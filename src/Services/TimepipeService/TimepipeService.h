#pragma once

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    class TimepipeService
        : public ServiceBase<TimepipeServiceInterface>
    {
    public:
        TimepipeService();
        ~TimepipeService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentInterfacePtr & _doc ) override;
        void removeTimepipe( const TimepipeInterfacePtr & _timepipe ) override;

    public:
        void tick( const UpdateContext * _context ) override;

    protected:
        struct TimepipeDesc
        {
            TimepipeInterfacePtr timepipe;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<TimepipeDesc> VectorTimepipe;
        VectorTimepipe m_timepipeAdd;
        VectorTimepipe m_timepipe;
    };
};
