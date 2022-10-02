#pragma once

#include "Interface/PersistentSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class ApplePersistentSystem
        : public ServiceBase<PersistentSystemInterface>
    {
    public:
        ApplePersistentSystem();
        ~ApplePersistentSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool getPersistentArguments( Char * _value, size_t _capacity ) const override;
        void setPersistentArguments( const Char * _value ) override;
        void removePersistentArguments() override;
    };
};
