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

    private:
        bool convert_( const FilePath & _input, const FilePath & _output );
    };
}
