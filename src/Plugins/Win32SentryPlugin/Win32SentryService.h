#pragma once

#include "Win32SentryInterface.h"

#include "Interface/LoggerInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ErrorLevel.h"
#include "Kernel/AssertionLevel.h"

namespace Mengine
{
    class Win32SentryService
        : public ServiceBase<Win32SentryServiceInterface>
    {
    public:
        Win32SentryService();
        ~Win32SentryService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void notifyAssertion_( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message );
        void notifyError_( const Char * _category, EErrorLevel _level, const Char * _file, int32_t _line, const Char * _message );
        void notifyBootstrapperCreateApplication_();

    protected:
        LoggerInterfacePtr m_sentryLogger;
    };
}