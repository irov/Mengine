#pragma once

#include "Kernel/Logger.h"
#include "Kernel/LoggerMessage.h"

namespace Mengine
{
    class TimeDogScope
    {
    public:
        TimeDogScope( const Char * _category, const Char * _file, int32_t _line, const Char * _function, const Char * _format, ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 6, 7 );
        ~TimeDogScope();

    private:
        TimeDogScope( const TimeDogScope & ) = delete;
        void operator = ( const TimeDogScope & ) = delete;

    protected:
        Char m_category[MENGINE_LOGGER_MAX_CATEGORY + 1] = {'\0'};
        Char m_message[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};

        const Char * m_file;
        const Char * m_function;
        int32_t m_line;

        double m_beginTime;
        bool m_enabled;
    };
}

#if defined(MENGINE_LOGGER_INFO_ENABLE)

#define TIMEDOG_SCOPE_II( Category, Index, ... ) Mengine::TimeDogScope MENGINE_PP_CONCATENATE(timedog_scope_, Index)( Category, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION, __VA_ARGS__ )
#define TIMEDOG_SCOPE_I( Category, Index, ... ) TIMEDOG_SCOPE_II( Category, Index, __VA_ARGS__ )
#define TIMEDOG_SCOPE( Category, ... ) TIMEDOG_SCOPE_I( Category, MENGINE_CODE_COUNTER, __VA_ARGS__ )

#else

#define TIMEDOG_SCOPE( Category, ... )

#endif
