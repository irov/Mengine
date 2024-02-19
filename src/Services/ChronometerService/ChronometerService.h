#pragma once

#include "Interface/ChronometerServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "Chronometer.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

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
        ChronometerInterfacePtr addChronometer( const LambdaChronometer & _lambda, const DocumentInterfacePtr & _doc ) override;
        bool removeChronometer( const ChronometerInterfacePtr & _chronometer ) override;

    protected:
        FactoryInterfacePtr m_factoryChronometer;

        typedef Vector<ChronometerPtr> VectorChronometers;
        VectorChronometers m_chronometers;
        VectorChronometers m_chronometersProcess;
    };
};
