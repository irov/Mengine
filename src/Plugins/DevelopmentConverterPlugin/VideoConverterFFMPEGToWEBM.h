#pragma once

#include "DevelopmentConverter.h"

namespace Mengine
{
    class VideoConverterFFMPEGToWEBM
        : public DevelopmentConverter
    {
    public:
        VideoConverterFFMPEGToWEBM();
        ~VideoConverterFFMPEGToWEBM() override;

    public:
        bool _initialize() override;

    public:
        bool convert() override;
    };
}
