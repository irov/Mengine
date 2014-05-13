#	pragma once

#	include "Codec/ImageDecoder.h"

#   include "Interface/ArchiveInterface.h"

namespace Menge
{
	class ImageDecoderETZ1 
		: public ImageDecoder
	{
	public:
		ImageDecoderETZ1();
		~ImageDecoderETZ1();

	public:
		bool _initialize() override;

	public:
		bool _prepareData() override;
		
	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;

	protected:
		ArchivatorInterfacePtr m_archivator;

		uint32_t m_uncompress_size;		
		uint32_t m_compress_size;
	};
}
