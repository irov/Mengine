#pragma once

#include "Interface/ServantInterface.h"

#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RandomizerInterface
        : public ServantInterface
    {
    public:
        virtual void setSeed( uint64_t _seed ) = 0;

    public:
        virtual uint32_t getRandom32( uint32_t _max ) const = 0;
        virtual uint32_t getRandomRange32( uint32_t _min, uint32_t _max ) const = 0;

        virtual int32_t getRandom32i( int32_t _max ) const = 0;
        virtual int32_t getRandomRange32i( int32_t _min, int32_t _max ) const = 0;

        virtual uint64_t getRandom64( uint64_t _max ) const = 0;
        virtual uint64_t getRandomRange64( uint64_t _min, uint64_t _max ) const = 0;

        virtual int64_t getRandom64i( int64_t _max ) const = 0;
        virtual int64_t getRandomRange64i( int64_t _min, int64_t _max ) const = 0;

        virtual float getRandomf( float _max ) const = 0;
        virtual float getRandomRangef( float _min, float _max ) const = 0;

        virtual double getRandomd( double _max ) const = 0;
        virtual double getRandomRanged( double _min, double _max ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RandomizerInterface> RandomizerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}