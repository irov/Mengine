#include "StdioLogger.h"

#include "Kernel/LoggerHelper.h"

#include <iostream>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    StdioLogger::StdioLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StdioLogger::~StdioLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void StdioLogger::log( const LoggerMessage & _message )
    {
        if( _message.flag & ELF_FLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( _message.file, _message.line, "%s[%u]", functionstamp, 0, MENGINE_MAX_PATH );
            std::cout.write( functionstamp, functionstampSize );
            std::cout.write( " ", 1 );
        }

        if( _message.flag & ELF_FLAG_TIMESTAMP )
        {
            Char timestamp[256] = {'\0'};
            size_t timestampSize = Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            std::cout.write( timestamp, timestampSize );
            std::cout.write( " ", 1 );
        }

        if( _message.flag & ELF_FLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );
            std::cout.write( threadstamp, threadstampSize );
            std::cout.write( " ", 1 );
        }

        if( _message.flag & ELF_FLAG_SYMBOL )
        {
            ELoggerLevel level = _message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            std::cout.write( &symbol, 1 );
            std::cout.write( " ", 1 );
        }

        if( _message.flag & ELF_FLAG_CATEGORY )
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
    void StdioLogger::flush()
    {
        std::cout.flush();
    }
    //////////////////////////////////////////////////////////////////////////
}
