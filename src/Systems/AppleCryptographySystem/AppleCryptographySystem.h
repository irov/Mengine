#pragma once

#include "Interface/CryptographySystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleCryptographySystem
        : public ServiceBase<CryptographySystemInterface>
    {
    public:
        AppleCryptographySystem();
        ~AppleCryptographySystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool generateRandomSeed( uint64_t * const _seed ) const override;
        bool generateRandomHexadecimal( size_t _length, Char * const _hexadecimal, bool _lowercase ) const override;
    };
};
