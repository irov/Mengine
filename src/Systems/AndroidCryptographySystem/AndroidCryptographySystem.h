#pragma once

#include "Interface/CryptographySystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AndroidCryptographySystem
        : public ServiceBase<CryptographySystemInterface>
    {
    public:
        AndroidCryptographySystem();
        ~AndroidCryptographySystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool generateRandomSeed( uint64_t * const _seed ) const override;
        bool generateRandomHexadecimal( Char * const _hexadecimal, size_t _capacity, bool _lowercase ) const override;
    };
};
