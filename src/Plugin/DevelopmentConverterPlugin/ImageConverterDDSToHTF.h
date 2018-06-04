#pragma once

#include "DevelopmentConverter.h"

namespace Mengine
{
	class ImageConverterDDSToHTF
		: public DevelopmentConverter
	{
	public:
		ImageConverterDDSToHTF();
		~ImageConverterDDSToHTF() override;

	public:
		bool _initialize() override;

    public:
		bool convert() override;        
    };
}	
