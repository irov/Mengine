#pragma once

#include "Interface/RandomizerInterface.h"

#include <utility>

namespace Mengine
{
    namespace Helper
    {
        template<class C>
        void shuffleVector( C & _vector, const RandomizerInterfacePtr & _randomizer )
        {
            typename C::size_type size = _vector.size();

            for( typename C::size_type index = 0; index != size; ++index )
            {
                uint32_t random_index = _randomizer->getRandom( size );

                std::swap( _vector[index], _vector[random_index] );
            }
        }
    }
}