#pragma once

#include "AppleSentryInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/AssertionLevel.h"
#include "Kernel/ErrorLevel.h"
#include "Interface/LoggerInterface.h"

namespace Mengine
{
    class AppleSentryService
        : public ServiceBase<AppleSentryServiceInterface>
    {
    public:
        AppleSentryService();
        ~AppleSentryService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void notifyCreateApplication_();
        void notifyAssertion_( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message );
        void notifyError_( const Char * _category, EErrorLevel _level, const Char * _file, int32_t _line, const Char * _message );
        void notifyEngineStop_();

    protected:
        LoggerInterfacePtr m_logger;
    };
}
