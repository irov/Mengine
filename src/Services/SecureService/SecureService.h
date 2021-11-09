#pragma once

#include "Interface/SecureServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SecureService
        : public ServiceBase<SecureServiceInterface>
    {
    public:
        SecureService();
        ~SecureService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        uint64_t getSecureHash() const override;

    public:
        void protectData( void * const _buffer, size_t _size ) const override;
        void unprotectData( void * const _buffer, size_t _size ) const override;

    protected:
        uint64_t m_secureHash;
    };
};
