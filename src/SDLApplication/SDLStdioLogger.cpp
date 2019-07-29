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
    void SDLStdioLogger::log( EMessageLevel _level, uint32_t _flag, const Char * _data, uint32_t _count )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _count );

        std::cout.write( _data, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::flush()
    {
        std::cout.flush();
    }
}
