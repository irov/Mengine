#pragma once

#include "Interface/RandomizerInterface.h"

#include "Kernel/Vector.h"
#include "Kernel/String.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdUtility.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class C>
        String vectorToString( const C & _vector )
        {
            String str;

            str += '[';

            for( const typename C::value_type & value : _vector )
            {
                str.append( value.c_str(), value.size() );

                str += ", ";
            }

            str += ']';

            return str;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class C>
        C exciseVector( C & _vector, uint32_t _count )
        {
            typename C::size_type size = _vector.size();

            if( size <= _count )
            {
                return std::move( _vector );
            }

            typename C::const_iterator it_begin = _vector.begin();
            typename C::const_iterator it_last = it_begin + _count;
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

                typename C::value_type & a = _vector[index];
                typename C::value_type & b = _vector[random_index];

                StdAlgorithm::swap( a, b );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
