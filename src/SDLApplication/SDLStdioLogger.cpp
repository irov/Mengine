#include "SDLStdioLogger.h"

#include <iostream>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLStdioLogger::SDLStdioLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLStdioLogger::~SDLStdioLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _color );

        std::cout.write( _data, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::flush()
    {
        std::cout.flush();
    }
}
