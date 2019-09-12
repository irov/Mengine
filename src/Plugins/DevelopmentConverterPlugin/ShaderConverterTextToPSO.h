#pragma once

#include "DevelopmentConverter.h"

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

    public:
        bool convert() override;
    };
}
