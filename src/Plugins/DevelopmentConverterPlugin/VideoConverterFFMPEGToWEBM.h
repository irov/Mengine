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

    protected:
        bool convert_( const FilePath & _input, const FilePath & _output );
    };
}
