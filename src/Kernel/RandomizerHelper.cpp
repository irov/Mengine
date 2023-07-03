#include "RandomizerHelper.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    namespace Helper
    {
        void randomizerShuffle( const RandomizerInterfacePtr & _randomizer, uint32_t * const _indices, uint32_t _count )
        {
            uint32_t enumerator = 0;
            Algorithm::for_each( _indices, _indices + _count, [&enumerator]( uint32_t & _value )
            {
                _value = enumerator++;
            } );

            for( uint32_t index = 0; index != _count; ++index )
            {
                uint32_t rnd_index = _randomizer->getRandom( _count );

                Algorithm::swap( _indices[index], _indices[rnd_index] );
            }
        }
    }
}