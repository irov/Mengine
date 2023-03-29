#pragma once

#include "Interface/ChronometerServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#if defined(MENGINE_DEBUG)
#   include "Kernel/Document.h"
#endif

#include "Config/Time.h"

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
        UniqueId addChronometer( const LambdaChronometer & _lambda, const DocumentPtr & _doc ) override;
        bool removeChronometer( UniqueId _id ) override;

    protected:
        TimeMilliseconds m_oldTime;

        struct ChronometerDesc
        {
            UniqueId id;
            LambdaChronometer lambda;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentPtr doc;
#endif
        };

        typedef Vector<ChronometerDesc> VectorChronometerDesc;
        VectorChronometerDesc m_chronometersProcess;
        VectorChronometerDesc m_chronometers;
    };
};
