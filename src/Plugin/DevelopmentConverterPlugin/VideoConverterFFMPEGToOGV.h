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
		bool initialize() override;

    public:
		bool convert() override;
        	
    protected:
		bool convert_( const FilePath & _input, const FilePath & _output );
    };
}	
