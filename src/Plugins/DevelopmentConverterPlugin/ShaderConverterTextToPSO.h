#pragma once

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class ShaderConverterTextToPSO
        : public DevelopmentConverter
    {
    public:
        ShaderConverterTextToPSO();
        ~ShaderConverterTextToPSO() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;
    };
}
