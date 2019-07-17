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
        HashType getSequreHash() const override;

    public:
        void protectData( void * _buffer, size_t _size ) const override;
        void unprotectData( void * _buffer, size_t _size ) const override;

    protected:
        HashType m_secureHash;
    };
};
