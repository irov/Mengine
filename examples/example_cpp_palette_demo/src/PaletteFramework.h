#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class PaletteFramework
        : public FrameworkBase
        , public Observable
    {
    public:
        PaletteFramework();
        ~PaletteFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
