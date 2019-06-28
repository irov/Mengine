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

    public:
        bool convert() override;

    private:
        bool convert_( const FilePath & _input, const FilePath & _output );
    };
}
