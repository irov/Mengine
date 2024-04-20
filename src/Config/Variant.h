#pragma once

#include "Config/Config.h"

#include <variant>
#include <functional>

namespace Mengine
{
    template<class ... Types>
    using Variant = std::variant<Types ...>;

    namespace Helper
    {
        template<class... Ts>
        struct Overloaded : Ts... 
        { 
            using Ts::operator()...;
        };

        template<class... Ts>
        Overloaded( Ts... ) -> Overloaded<Ts...>;

        template<class ... Types, class ... Lambdas>
        void visit( const Variant<Types ...> & _variant, Lambdas ... _lambda )
        {
            std::visit( Helper::Overloaded{_lambda ...}, _variant );
        }

        template<class ... Types, class T>
        bool get( const Variant<Types ...> & _variant, T * const _value )
        {
            try
            {
                *_value = std::get<T>( _variant );

                return true;
            }
            catch( const std::bad_variant_access & )
            {
            }

            return false;
        }
    }
}
