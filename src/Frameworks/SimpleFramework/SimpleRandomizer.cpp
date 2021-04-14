#include "SimpleRandomizer.h"

#include "Interface/RandomizerInterface.h"
#include "Interface/PlayerServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void setSimpleRandomizerSeed( uint32_t _seed )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            randomizer->setSeed( _seed );
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getSimpleRandomizerRandom( uint32_t _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            uint32_t r = randomizer->getRandom( _max );

            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getSimpleRandomizerRandomRange( uint32_t _min, uint32_t _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            uint32_t r = randomizer->getRandomRange( _min, _max );

            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t getSimpleRandomizerRandomi( int32_t _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            int32_t r = randomizer->getRandomi( _max );

            return r;
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t getSimpleRandomizerRandomRangei( int32_t _min, int32_t _max )
        {
            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            int32_t r = randomizer->getRandomRangei( _min, _max );

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
    }
}