#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"


namespace Mengine
{
    class AppLovinRewardCallback{
        public:
            virtual void receivedReward(long amount) = 0;
    
    };
}
