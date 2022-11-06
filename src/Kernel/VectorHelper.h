#pragma once

#include "Interface/RandomizerInterface.h"

#include <utility>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class C>
        C exciseVector( C & _vector, uint32_t _count )
        {
            if( _vector.size() <= _count )
            {
                return std::move( _vector );
            }

            typename C::const_iterator it_last = _vector.begin() + _count;
            typename C::const_iterator it_end = _vector.end();

            C tail( it_last, it_end );

            _vector.erase( it_last, it_end );

            C head = std::move( _vector );
            _vector = std::move( tail );

            return std::move( head );
        }
        //////////////////////////////////////////////////////////////////////////
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
        //////////////////////////////////////////////////////////////////////////
    }
}
