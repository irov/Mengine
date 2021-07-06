#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class GraphicsFramework
        : public FrameworkBase
    {
        DECLARE_FACTORABLE( GraphicsFramework );

    public:
        GraphicsFramework();
        ~GraphicsFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
