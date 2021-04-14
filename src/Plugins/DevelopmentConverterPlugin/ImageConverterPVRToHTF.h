#pragma once

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class ImageConverterPVRToHTF
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( ImageConverterPVRToHTF );

    public:
        ImageConverterPVRToHTF();
        ~ImageConverterPVRToHTF() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;
    };
}
