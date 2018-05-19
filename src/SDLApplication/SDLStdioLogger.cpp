#include "SDLStdioLogger.h"

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
    void SDLStdioLogger::log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count )
    {
		(void)_level;
        (void)_flag;
		(void)_count;

		std::cout.write( _data, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::flush()
    {
    }
}	// namespace Mengine
