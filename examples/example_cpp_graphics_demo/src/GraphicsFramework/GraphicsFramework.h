#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class GraphicsFramework
        : public FrameworkBase
        , public Observable
    {
    public:
        GraphicsFramework();
        ~GraphicsFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
