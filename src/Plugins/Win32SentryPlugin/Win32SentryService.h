#pragma once

#include "Win32SentryInterface.h"

#include "Win32SentryLoggerCapture.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ErrorLevel.h"

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
        void notifyEngineStop_();

    protected:
        Win32SentryLoggerCapturePtr m_loggerCapture;
    };
}