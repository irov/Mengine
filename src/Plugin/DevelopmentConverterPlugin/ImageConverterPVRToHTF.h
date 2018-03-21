#pragma once

#include "DevelopmentConverter.h"

namespace Mengine
{
	class ImageConverterPVRToHTF
		: public DevelopmentConverter
	{
	public:
		ImageConverterPVRToHTF();
		~ImageConverterPVRToHTF() override;

	public:
		bool initialize() override;

    public:
		bool convert() override;
    };
}	
