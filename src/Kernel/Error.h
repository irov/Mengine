#pragma once

#include "Kernel/ErrorLevel.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

#if defined(MENGINE_DEBUG)
#   include "Config/StdIO.h"
#endif

namespace Mengine
{
    namespace Helper
    {
        class ErrorOperator
        {
        public:
            ErrorOperator( const Char * _category, EErrorLevel _level, const Char * _file, uint32_t _line );
            ~ErrorOperator();

        public:
            void operator()( const Char * _format, ... ) const MENGINE_ATTRIBUTE_FORMAT_STRING( 2, 3 );

        protected:
            const Char * m_category;
            EErrorLevel m_level;
            const Char * m_file;
            uint32_t m_line;
        };
    }
}

#if defined(MENGINE_DEBUG)
#   define MENGINE_ERROR_CALL( ... ) (__VA_ARGS__ ) ;static_assert(sizeof(printf(__VA_ARGS__)))
#else
#   define MENGINE_ERROR_CALL( ... ) (__VA_ARGS__ ) ;static_assert(sizeof(printf(__VA_ARGS__)))
#endif

//////////////////////////////////////////////////////////////////////////
#define MENGINE_ERROR_MESSAGE(...) Mengine::Helper::ErrorOperator( MENGINE_CODE_LIBRARY, ERROR_LEVEL_MESSAGE, MENGINE_CODE_FILE, MENGINE_CODE_LINE ) MENGINE_ERROR_CALL(__VA_ARGS__)
#define MENGINE_ERROR_FATAL(...) Mengine::Helper::ErrorOperator( MENGINE_CODE_LIBRARY, ERROR_LEVEL_FATAL, MENGINE_CODE_FILE, MENGINE_CODE_LINE ) MENGINE_ERROR_CALL(__VA_ARGS__)
//////////////////////////////////////////////////////////////////////////