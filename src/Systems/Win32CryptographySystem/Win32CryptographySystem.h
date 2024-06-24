#pragma once

#include "Interface/CryptographySystemInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32CryptographySystem
        : public ServiceBase<CryptographySystemInterface>
    {
    public:
        Win32CryptographySystem();
        ~Win32CryptographySystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool generateRandomSeed( uint64_t * const _seed ) const override;
        bool generateRandomHexadecimal( uint32_t _length, Char * const _hexadecimal, bool _lowercase ) const override;

    protected:
        HCRYPTPROV m_hCryptProv;
    };
};
