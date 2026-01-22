#pragma once

#include "Interface/SecureServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SecureService
        : public ServiceBase<SecureServiceInterface>
    {
        DECLARE_FACTORABLE( SecureService );

    public:
        SecureService();
        ~SecureService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void protectData( uint32_t _complexity, const void * _in, size_t _size, void * const _out ) const override;
        void unprotectData( uint32_t _complexity, const void * _in, size_t _size, void * const _out ) const override;

    protected:
        uint64_t m_secureHash[1] = {MENGINE_UINT64_C( 0 )};
    };
};
