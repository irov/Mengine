#pragma once

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Atomic.h"

namespace Mengine
{
    class EnumeratorService
        : public ServiceBase<EnumeratorServiceInterface>
    {
        DECLARE_FACTORABLE( EnumeratorService );

    public:
        EnumeratorService();
        ~EnumeratorService() override;

    public:
        UniqueId generateUniqueIdentity() override;

    protected:
        AtomicUInt32 m_enumerator;
    };
}