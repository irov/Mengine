#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class PaletteFramework
        : public FrameworkBase
    {
        DECLARE_FACTORABLE( PaletteFramework );

    public:
        PaletteFramework();
        ~PaletteFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
