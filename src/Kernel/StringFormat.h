#pragma once

#include "Kernel/String.h"
#include "Kernel/VectorString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        uint32_t getStringFormatExpectedArgs( const Char * _format, size_t _size );
        bool fillStringFormat( const Char * _format, size_t _size, const VectorString & _arguments, String * const _out );

        typedef Lambda<bool( const String & _value, String * const _out )> LambdaStringTag;
        bool fillStringTag( const String & _text, const Char * _tag, const LambdaStringTag & _lambda, String * const _out, bool * const _matched );
    }
}