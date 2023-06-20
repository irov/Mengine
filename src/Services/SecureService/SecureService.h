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
        void protectData( const void * _in, size_t _size, void * const _out ) const override;
        void unprotectData( const void * _in, size_t _size, void * const _out ) const override;

    protected:
        uint64_t m_secureHash;
    };
};
