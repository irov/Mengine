#pragma once

#include "SentryInterface.h"

#include "SentryLoggerCapture.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ErrorLevel.h"

namespace Mengine
{
    class Win32SentryService
        : public ServiceBase<SentryServiceInterface>
    {
    public:
        Win32SentryService();
        ~Win32SentryService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void notifyAssertion_( EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message );
        void notifyError_( EErrorLevel _level, const Char * _file, int32_t _line, const Char * _message );
        void notifyCreateApplication_();

    protected:
        SentryLoggerCapturePtr m_loggerCapture;
    };
}