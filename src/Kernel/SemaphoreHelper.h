#pragma once

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        void activateSemaphore( const ConstString & _name );
        void deactivateSemaphore( const ConstString & _name );

        typedef Lambda<void()> LambdaSemaphore;
        void waitSemaphore( const ConstString & _name, const LambdaSemaphore & _semaphore );
    }
}