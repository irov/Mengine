#include "ToolLogger.h"
#include "ToolUtils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ToolLogger::ToolLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ToolLogger::~ToolLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolLogger::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _color );

        printf( "%.*s"
            , _size
            , _data
        );
    };
}