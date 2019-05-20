#include "MessageBoxLogger.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MessageBoxLogger::MessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MessageBoxLogger::~MessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MessageBoxLogger::log( EMessageLevel _level, uint32_t _flag, const Char * _data, uint32_t _count )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _count );

        MessageBoxA( NULL, _data, "Critical Error", MB_OK );
    }
}
