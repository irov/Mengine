#pragma once

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class ImageConverterPNGToACF
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( ImageConverterPNGToACF );

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
