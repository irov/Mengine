#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class CastleRoyalFramework
        : public FrameworkBase
    {
        DECLARE_FACTORABLE( CastleRoyalFramework );

    public:
        CastleRoyalFramework();
        ~CastleRoyalFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
