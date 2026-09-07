#pragma once

#include "Kernel/String.h"
#include "Kernel/Stringstream.h"

#include "Config/StdUtility.h"
#include "Config/TypeTraits.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            void outputArg( Stringstream & _stream, T & _arg )
            {
                if constexpr( requires ( Stringstream & _output, const TypeTraits::decay_t<T> & _value ) { _output << _value; } )
                {
                    _stream << StdUtility::forward<T>( _arg );
                }
                else
                {
                    _stream << "UNKNOWN";
                }
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        template<class ... Args>
        String argsToString( Args && ... _args )
        {
            Stringstream stream;
            ((Detail::outputArg<Args>( stream, _args ), stream << ", "), ...);

            String result = stream.str();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}