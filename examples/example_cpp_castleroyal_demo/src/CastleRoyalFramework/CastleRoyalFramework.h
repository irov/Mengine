#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class CastleRoyalFramework
        : public FrameworkBase
    {
    public:
        CastleRoyalFramework();
        ~CastleRoyalFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
