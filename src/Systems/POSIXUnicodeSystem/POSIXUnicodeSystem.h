#pragma once

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class POSIXUnicodeSystem
        : public ServiceBase<UnicodeSystemInterface>
    {
    public:
        POSIXUnicodeSystem();
        ~POSIXUnicodeSystem() override;

    public:
        const ServiceRequiredList & requiredServices() const override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size ) override;
        bool utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _sizeUnicode ) override;

    protected:
        ThreadMutexInterfacePtr m_mutex;
    };
}
