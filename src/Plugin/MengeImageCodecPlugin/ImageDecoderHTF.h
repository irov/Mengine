#	pragma once

#	include "Codec/ImageDecoder.h"

#   include "Interface/ArchiveInterface.h"

#	include "Core/Magic.h"

namespace Menge
{
	DECLARE_MAGIC_NUMBER(MAGIC_HTF, 'H', 'T', 'F', '1', 1);

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
		size_t decode( void * _buffer, size_t _bufferSize ) override;
	
	private:       
		ArchivatorInterfacePtr m_archivator;

        uint32_t m_uncompress_size;
        uint32_t m_compress_size;
	};
}	// namespace Menge