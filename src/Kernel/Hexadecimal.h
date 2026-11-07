#pragma once

#include "Kernel/Data.h"
#include "Kernel/String.h"

#include "Config/Typedef.h"
#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EHexadecimalFlags : uint32_t
    {
        EHF_LOWERCASE = 0,
        EHF_UPPERCASE = 1 << 0,
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        bool encodeHexadecimal( const void * _data, size_t _datasize, Char * const _hexadecimal, size_t _capacity, uint32_t _flags, size_t * const _outsize );
        bool encodeHexadecimal( const Data & _data, String * const _hexadecimal, uint32_t _flags );

        bool decodeHexadecimal( const Char * _hexadecimal, size_t _size, void * const _data, size_t _capacity, size_t * const _outsize );
        bool decodeHexadecimal( const String & _hexadecimal, Data * const _data );
    }
}