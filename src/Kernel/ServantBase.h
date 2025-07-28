#pragma once

#include "Config/Config.h"

namespace Mengine
{
    template<class T>
    class ServantBase
        : public T
    {
    public:
        ServantBase()
        {
        }

        ~ServantBase() override
        {
        }
    };
}