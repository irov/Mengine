#include "SDLStdioLogger.h"

#include "Config/Config.h"

#include "SDL_log.h"

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
    void SDLStdioLogger::log( ELoggerLevel _level, uint32_t _flag, const Char * _data, size_t _count )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );

        std::cout.write( _data, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::flush()
    {
        std::cout.flush();
    }
}
