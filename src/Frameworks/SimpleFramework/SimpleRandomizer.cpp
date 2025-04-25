#include "SimpleRandomizer.h"

#include "Interface/RandomizerInterface.h"
#include "Interface/PlayerServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void setSimpleRandomizerSeed( uint64_t _seed )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            randomizer->setSeed( _seed );
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getSimpleRandomizerRandom32( uint32_t _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            uint32_t r = randomizer->getRandom32( _max );

            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getSimpleRandomizerRandomRange32( uint32_t _min, uint32_t _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            uint32_t r = randomizer->getRandomRange32( _min, _max );

            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t getSimpleRandomizerRandom32i( int32_t _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            int32_t r = randomizer->getRandom32i( _max );

            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t getSimpleRandomizerRandomRange32i( int32_t _min, int32_t _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            int32_t r = randomizer->getRandomRange32i( _min, _max );

            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        float getSimpleRandomizerRandomf( float _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            float r = randomizer->getRandomf( _max );

            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        float getSimpleRandomizerRandomRangef( float _min, float _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            float r = randomizer->getRandomRangef( _min, _max );

            return r;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}