#pragma once

#   include "Interface/ArchiveInterface.h"

#include "Codec/ImageEncoder.h"

namespace Mengine
{
	class ImageEncoderACF
		: public ImageEncoder
	{
	public:
		ImageEncoderACF();
		~ImageEncoderACF();

	public:
		bool _initialize() override;

	public:
		size_t encode( const void * _buffer, size_t _size, const CodecDataInfo* _bufferDataInfo ) override;

	protected:
		ArchivatorInterfacePtr m_archivator;
	};
}	
