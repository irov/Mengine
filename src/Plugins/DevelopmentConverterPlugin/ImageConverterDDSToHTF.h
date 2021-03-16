#pragma once

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class ImageConverterDDSToHTF
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( ImageConverterDDSToHTF );

    public:
        ImageConverterDDSToHTF();
        ~ImageConverterDDSToHTF() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;
    };
}
