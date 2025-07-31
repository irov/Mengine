#pragma once

#include "Interface/CryptographySystemInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class UWPCryptographySystem
        : public ServiceBase<CryptographySystemInterface>
    {
    public:
        UWPCryptographySystem();
        ~UWPCryptographySystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool generateRandomSeed( uint64_t * const _seed ) const override;
        bool generateRandomHexadecimal( size_t _length, Char * const _hexadecimal, bool _lowercase ) const override;
    };
};
