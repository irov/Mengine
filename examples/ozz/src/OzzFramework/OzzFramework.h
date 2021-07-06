#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class OzzFramework
        : public FrameworkBase
    {
        DECLARE_FACTORABLE( OzzFramework );

    public:
        OzzFramework();
        ~OzzFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
