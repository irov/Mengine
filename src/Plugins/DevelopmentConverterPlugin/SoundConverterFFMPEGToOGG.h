#pragma once

#include "DevelopmentConverter.h"

namespace Mengine
{
    class SoundConverterFFMPEGToOGG
        : public DevelopmentConverter
    {
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
