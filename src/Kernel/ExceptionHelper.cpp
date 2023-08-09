#include "ExceptionHelper.h"
#include "Exception.h"

#include "Config/StdIO.h"
#include "Config/StdArg.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ExceptionHelper::ExceptionHelper( const Char * _file, uint32_t _line ) noexcept
        : m_file( _file )
        , m_line( _line )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ExceptionHelper::~ExceptionHelper() noexcept
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ExceptionHelper::operator () ( const Char * _format, ... ) const
    {
        Char foramt_msg[MENGINE_EXCEPTION_MAX_MESSAGE] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( foramt_msg, MENGINE_EXCEPTION_MAX_MESSAGE - 1, _format, args );
        MENGINE_UNUSED( size_vsnprintf );

        MENGINE_VA_LIST_END( args );

        Char exception_msg[MENGINE_EXCEPTION_MAX_MESSAGE] = {'\0'};

        MENGINE_STRCAT( exception_msg, "message: " );
        MENGINE_STRCAT( exception_msg, foramt_msg );
        MENGINE_STRCAT( exception_msg, "\n" );

        MENGINE_STRCAT( exception_msg, "file: " );
        MENGINE_STRCAT( exception_msg, m_file );
        MENGINE_STRCAT( exception_msg, "\n" );

        Char format_line[32] = {'\0'};
        MENGINE_SNPRINTF( format_line, 16, "%u", m_line );

        MENGINE_STRCAT( exception_msg, "line: " );
        MENGINE_STRCAT( exception_msg, format_line );
        MENGINE_STRCAT( exception_msg, "\n" );

        throw Exception( exception_msg );
    }
    //////////////////////////////////////////////////////////////////////////
}
