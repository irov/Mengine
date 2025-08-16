#pragma once

#include "Kernel/String.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    namespace Helper
    {
        template<typename ... Args>
        String argsToString( Args && ... _args )
        {
            Stringstream stream;
            ((stream << std::forward<Args>( _args ) << ", "), ...);

            return stream.str();
        }
    }
}