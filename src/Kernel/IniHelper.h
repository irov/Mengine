#pragma once

#include "Interface/OutputStreamInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Char.h"
#include "Config/String.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const Char * _value, size_t _size );
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const ConstString & _value );
        bool writeIniSetting( const OutputStreamInterfacePtr & _stream, const Char * _key, const String & _value );
        bool writeIniSection( const OutputStreamInterfacePtr & _stream, const Char * _section, uint32_t _sectionSize );
        //////////////////////////////////////////////////////////////////////////
        template<class T, size_t I_Size>
        static bool writeIniSection( const OutputStreamInterfacePtr & _stream, T( &_section )[I_Size] )
        {
            bool result = writeIniSection( _stream, _section, sizeof( _section ) - 1 );

            return result;
        }
    }
}