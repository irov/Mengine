#include "Win32MessageBoxLogger.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32MessageBoxLogger::Win32MessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32MessageBoxLogger::~Win32MessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MessageBoxLogger::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _count )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _color );
        MENGINE_UNUSED( _count );

        ::MessageBoxA( NULL, _data, "Critical Error", MB_OK );
    }
    //////////////////////////////////////////////////////////////////////////
}
