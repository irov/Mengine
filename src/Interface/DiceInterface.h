#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/RandomizerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DiceInterface
        : public ServantInterface
    {
    public:
        virtual void setRandomizer( const RandomizerInterfacePtr & _randomizer ) = 0;
        virtual const RandomizerInterfacePtr & getRandomizer() const = 0;

    public:
        virtual void setChance( float _chance ) = 0;
        virtual float getChance() const = 0;

    public:
        virtual bool roll() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DiceInterface> DiceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}