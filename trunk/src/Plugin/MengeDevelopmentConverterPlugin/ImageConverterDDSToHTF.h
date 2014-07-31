#	pragma once

#	include "DevelopmentConverter.h"

namespace Menge
{
	class ImageConverterDDSToHTF
		: public DevelopmentConverter
	{
	public:
		ImageConverterDDSToHTF();
		~ImageConverterDDSToHTF();

	public:
		bool initialize() override;

    public:
		bool convert() override;
    };
}	// namespace Menge
