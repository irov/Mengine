#pragma once

#include "DevelopmentConverter.h"

namespace Mengine
{
    class ShaderConverterTextToVSO
        : public DevelopmentConverter
    {
    public:
        ShaderConverterTextToVSO();
        ~ShaderConverterTextToVSO() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;
    };
}
