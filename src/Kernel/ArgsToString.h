#pragma once

#include "Kernel/String.h"
#include "Kernel/Stringstream.h"

#include <type_traits>

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            // SFINAE helper to check if type can be streamed
            // Check with std::decay_t<T> as const& to match real usage patterns
            template<typename T, typename = void>
            struct is_streamable : std::false_type {};

            template<typename T>
            struct is_streamable<T, std::void_t<decltype( std::declval<Stringstream &>() << std::declval<const std::decay_t<T> &>() )>> : std::true_type {};

            // Helper function to output value if streamable
            template<typename T>
            typename std::enable_if<is_streamable<T>::value, Stringstream &>::type
            outputArg( Stringstream & stream, T && arg )
            {
                stream << std::forward<T>( arg );
                return stream;
            }

            // Helper function to output "UNKNOWN" if not streamable
            template<typename T>
            typename std::enable_if<!is_streamable<T>::value, Stringstream &>::type
            outputArg( Stringstream & stream, T && /*arg*/ )
            {
                stream << "UNKNOWN";
                return stream;
            }
        }

        template<typename ... Args>
        String argsToString( Args && ... _args )
        {
            Stringstream stream;
            ((Detail::outputArg( stream, std::forward<Args>( _args ) ), stream << ", "), ...);

            return stream.str();
        }
    }
}