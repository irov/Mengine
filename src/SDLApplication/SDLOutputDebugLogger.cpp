#include "SDLOutputDebugLogger.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLOutputDebugLogger::SDLOutputDebugLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLOutputDebugLogger::~SDLOutputDebugLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLOutputDebugLogger::log( ELoggerLevel _level, uint32_t _filter, uint32_t _color, const Char * _data, size_t _count )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _filter );
        MENGINE_UNUSED( _color );
        MENGINE_UNUSED( _count );

        Char message[4096] = {'0'};
        MENGINE_SNPRINTF( message, 4096, "%.*s", _count, _data );

        ::OutputDebugStringA( message );
    }
    //////////////////////////////////////////////////////////////////////////
}
