#pragma once

#include "Kernel/DevelopmentConverter.h"

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
        void _finalize() override;

    public:
        bool convert() override;
    };
}
