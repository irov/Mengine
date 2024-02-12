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
        bool generateRandomHexadecimal( uint32_t _length, Char * const _hexadecimal ) const override;
    };
};
