#pragma once

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class VideoConverterFFMPEGToOGVA
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( VideoConverterFFMPEGToOGVA );

    public:
        VideoConverterFFMPEGToOGVA();
        ~VideoConverterFFMPEGToOGVA() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;
    };
}
