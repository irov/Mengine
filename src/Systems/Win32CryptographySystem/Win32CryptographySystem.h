#pragma once

#include "Interface/CryptographySystemInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32CryptographySystem
        : public ServiceBase<CryptographySystemInterface>
    {
        DECLARE_FACTORABLE( Win32CryptographySystem );

    public:
        Win32CryptographySystem();
        ~Win32CryptographySystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool generateRandomSeed( uint64_t * const _seed ) const override;
        bool generateRandomHexadecimal( Char * const _hexadecimal, size_t _capacity, bool _lowercase ) const override;

    protected:
        HCRYPTPROV m_hCryptProvider;
    };
};
