#pragma once

#include "Kernel/String.h"
#include "Kernel/VectorString.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    namespace Helper
    {
        String stringFormat( MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... );

        void split( VectorString & _outStrings, const String & _str, bool _trimDelims, const String & _delim );
        void wsplit( VectorWString & _outStrings, const WString & _str, bool _trimDelims, const WString & _delim );

        void split2( VectorString & _outStrings, const String & _str, bool _trimDelims, const VectorString & _delims );
        void u32split2( VectorU32String & _outStrings, const U32String & _str, bool _trimDelims, const VectorU32String & _delims );

        bool u32split3( VectorU32String & _outStrings, const U32String & _str, const VectorU32String & _delims );

        void join( const String & _delim, const VectorString & _stringArray, String & _outString );
        void trim( String & str, bool left = true, bool right = true );
    }
};
