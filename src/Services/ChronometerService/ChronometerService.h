#pragma once

#include "Interface/ChronometerServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    class ChronometerService
        : public ServiceBase<ChronometerServiceInterface>
    {
    public:
        ChronometerService();
        ~ChronometerService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void update() override;

    public:
        UniqueId addChronometer( const LambdaChronometer & _lambda, const DocumentInterfacePtr & _doc ) override;
        bool removeChronometer( UniqueId _id ) override;

    protected:
        Timestamp m_oldTime;

        struct ChronometerDesc
        {
            UniqueId id;
            LambdaChronometer lambda;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<ChronometerDesc> VectorChronometerDesc;
        VectorChronometerDesc m_chronometersProcess;
        VectorChronometerDesc m_chronometers;
    };
};
