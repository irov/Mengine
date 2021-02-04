#pragma once

#include "Interface/OutputStreamInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, uint32_t _value );
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, float _value );
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const Char * _value, size_t _size );
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const ConstString & _value );
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const String & _value );
        bool writeIniSection( const OutputStreamInterfacePtr & _stream, const Char * _section );
    }
}