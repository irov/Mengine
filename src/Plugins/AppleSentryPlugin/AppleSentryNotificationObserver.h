#pragma once

#include "Kernel/AssertionLevel.h"
#include "Kernel/ErrorLevel.h"

namespace Mengine
{
    class AppleSentryNotificationObserver
    {
    public:
        AppleSentryNotificationObserver();
        ~AppleSentryNotificationObserver();

    public:
        bool initialize();
        void finalize();
        void setupApplicationScope();

    protected:
        void notifyCreateApplication_();
        void notifyAssertion_( const Char * _category, EAssertionLevel _level, const Char * _test, const Char * _file, int32_t _line, const Char * _message );
        void notifyError_( const Char * _category, EErrorLevel _level, const Char * _file, int32_t _line, const Char * _message );
        void notifyEngineStop_();
    };
}
