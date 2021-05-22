#pragma once

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class ShaderConverterTextToVSO11
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( ShaderConverterTextToVSO11 );

    public:
        ShaderConverterTextToVSO11();
        ~ShaderConverterTextToVSO11() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;
    };
}
