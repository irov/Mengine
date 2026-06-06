#pragma once

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class ShaderConverterTextToVSO3
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( ShaderConverterTextToVSO3 );

    public:
        ShaderConverterTextToVSO3();
        ~ShaderConverterTextToVSO3() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;
    };
}
