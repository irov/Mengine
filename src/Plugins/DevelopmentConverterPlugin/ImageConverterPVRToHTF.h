#pragma once

#include "DevelopmentConverter.h"

namespace Mengine
{
    class ImageConverterPVRToHTF
        : public DevelopmentConverter
    {
    public:
        ImageConverterPVRToHTF();
        ~ImageConverterPVRToHTF() override;

    public:
        bool _initialize() override;

    public:
        bool convert() override;
    };
}
