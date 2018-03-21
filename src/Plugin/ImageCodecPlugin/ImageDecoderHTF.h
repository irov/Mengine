#pragma once

#include "Codec/ImageDecoder.h"

#include "Interface/ArchiveInterface.h"

#include "Core/Magic.h"

namespace Mengine
{
	class ImageDecoderHTF
		: public ImageDecoder
	{
	public:
		ImageDecoderHTF();
		~ImageDecoderHTF();

	public:
		bool _initialize() override;
		
	public:
		bool _prepareData() override;

	public:
		size_t _decode( void * _buffer, size_t _bufferSize ) override;
	
	private:       
		ArchivatorInterfacePtr m_archivator;
	};
}	
