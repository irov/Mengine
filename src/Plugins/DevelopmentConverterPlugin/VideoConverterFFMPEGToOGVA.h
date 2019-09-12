#pragma once

#include "DevelopmentConverter.h"

namespace Mengine
{
    class VideoConverterFFMPEGToOGVA
        : public DevelopmentConverter
    {
    public:
        VideoConverterFFMPEGToOGVA();
        ~VideoConverterFFMPEGToOGVA() override;

    public:
        bool _initialize() override;

    public:
        bool convert() override;
    };
}
