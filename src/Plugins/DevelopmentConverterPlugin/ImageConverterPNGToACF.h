#pragma once

#include "DevelopmentConverter.h"

namespace Mengine
{
    class ImageConverterPNGToACF
        : public DevelopmentConverter
    {
    public:
        ImageConverterPNGToACF();
        ~ImageConverterPNGToACF() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;
    };
}
