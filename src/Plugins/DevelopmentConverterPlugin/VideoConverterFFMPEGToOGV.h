#pragma once

#include "DevelopmentConverter.h"

namespace Mengine
{
    class VideoConverterFFMPEGToOGV
        : public DevelopmentConverter
    {
    public:
        VideoConverterFFMPEGToOGV();
        ~VideoConverterFFMPEGToOGV() override;

    public:
        bool _initialize() override;

    public:
        bool convert() override;
    };
}
