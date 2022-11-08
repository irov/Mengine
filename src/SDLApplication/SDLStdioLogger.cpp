#include "SDLStdioLogger.h"

#include "Kernel/LoggerHelper.h"

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
    void SDLStdioLogger::log( const LoggerMessage & _message )
    {
        Char timestamp[256] = {'\0'};
        size_t timestampSize = Helper::makeLoggerTimestamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 256 );
        std::cout.write( timestamp, timestampSize );
        std::cout.write( " ", 1 );

        if( _message.category.empty() == false )
        {
            const Char * category_str = _message.category.c_str();
            size_t category_size = _message.category.size();

            std::cout.write( "[", 1 );
            std::cout.write( category_str, category_size );
            std::cout.write( "]", 1 );
            std::cout.write( " ", 1 );
        }

        const Char * data = _message.data;
        size_t size = _message.size;

        std::cout.write( data, size );
        std::cout.write( "\n", 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLStdioLogger::flush()
    {
        std::cout.flush();
    }
    //////////////////////////////////////////////////////////////////////////
}
