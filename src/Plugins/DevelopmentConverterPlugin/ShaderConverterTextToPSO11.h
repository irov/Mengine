#pragma once

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class ShaderConverterTextToPSO11
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( ShaderConverterTextToPSO11 );

    public:
        ShaderConverterTextToPSO11();
        ~ShaderConverterTextToPSO11() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;
    };
}
