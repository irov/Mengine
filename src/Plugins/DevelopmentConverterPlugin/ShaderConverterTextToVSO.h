#pragma once

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class ShaderConverterTextToVSO
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( ShaderConverterTextToVSO );

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
