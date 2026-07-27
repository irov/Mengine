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
        DECLARE_FACTORABLE( TimepipeService );

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
        UniqueId addTimebeginCallback( const LambdaTimepipeCallback & _callback, const DocumentInterfacePtr & _doc ) override;
        bool removeTimebeginCallback( UniqueId _id ) override;
        UniqueId addTimeendCallback( const LambdaTimepipeCallback & _callback, const DocumentInterfacePtr & _doc ) override;
        bool removeTimeendCallback( UniqueId _id ) override;

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

        struct TimepipeCallbackDesc
        {
            UniqueId id;
            LambdaTimepipeCallback callback;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<TimepipeCallbackDesc> VectorTimepipeCallbacks;

    protected:
        VectorTimepipeCallbacks m_timebeginCallbacks;
        VectorTimepipeCallbacks m_timebeginCallbacksAux;
        VectorTimepipeCallbacks m_timeendCallbacks;
        VectorTimepipeCallbacks m_timeendCallbacksAux;
    };
};
