#pragma once

#include "Interface/ChronometerServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/String.h"

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
        uint32_t addChronometer( const LambdaChronometer & _lambda, const Char * _doc ) override;
        bool removeChronometer( uint32_t _id ) override;

    protected:
        uint64_t m_oldTime;

        struct ChronometerDesc
        {
            uint32_t id;
            LambdaChronometer lambda;
#ifndef NDEBUG
            String doc;
#endif
        };

        typedef Vector<ChronometerDesc> VectorChronometerDesc;
        VectorChronometerDesc m_chronometersProcess;
        VectorChronometerDesc m_chronometers;
    };
};
