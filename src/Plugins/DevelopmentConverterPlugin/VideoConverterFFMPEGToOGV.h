#pragma once

#include "Kernel/DevelopmentConverter.h"

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
        void _finalize() override;

    public:
        bool convert() override;
    };
}
