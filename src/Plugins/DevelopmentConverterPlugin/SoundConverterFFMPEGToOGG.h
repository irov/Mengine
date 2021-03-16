#pragma once

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class SoundConverterFFMPEGToOGG
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( SoundConverterFFMPEGToOGG );

    public:
        SoundConverterFFMPEGToOGG();
        ~SoundConverterFFMPEGToOGG() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;
    };
}
