#pragma once

#include "DevelopmentConverter.h"

#include "Interface/ArchiveInterface.h"

namespace Mengine
{
	class ImageConverterPNGToACF
		: public DevelopmentConverter
	{
	public:
		ImageConverterPNGToACF();
		~ImageConverterPNGToACF() override;

	public:
		bool initialize() override;

    public:
		bool convert() override;
    };
}	
