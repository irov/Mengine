#pragma once

#include "Interface/ServantInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RandomizerInterface
        : public ServantInterface
    {
    public:
        virtual void setSeed( uint32_t _seed ) = 0;

    public:
        virtual uint32_t getRandom( uint32_t _max ) const = 0;
        virtual uint32_t getRandomRange( uint32_t _min, uint32_t _max ) const = 0;

        virtual int32_t getRandomi( int32_t _max ) const = 0;
        virtual int32_t getRandomRangei( int32_t _min, int32_t _max ) const = 0;

        virtual float getRandomf( float _max ) const = 0;
        virtual float getRandomRangef( float _min, float _max ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RandomizerInterface> RandomizerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}