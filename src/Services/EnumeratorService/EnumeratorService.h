#pragma once

#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class EnumeratorService
        : public ServiceBase<EnumeratorServiceInterface>
    {
    public:
        EnumeratorService();
        ~EnumeratorService() override;

    public:
        uint32_t generateUniqueIdentity() override;

    protected:
        uint32_t m_enumerator;
    };
}