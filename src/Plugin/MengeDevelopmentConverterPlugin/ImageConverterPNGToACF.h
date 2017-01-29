#	pragma once

#	include "DevelopmentConverter.h"

#   include "Interface/ArchiveInterface.h"

namespace Menge
{
	class ImageConverterPNGToACF
		: public DevelopmentConverter
	{
	public:
		ImageConverterPNGToACF();
		~ImageConverterPNGToACF();

	public:
		bool initialize() override;

    public:
		bool convert() override;
    };
}	// namespace Menge
