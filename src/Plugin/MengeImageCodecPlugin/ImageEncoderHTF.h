#pragma once

#   include "Interface/ArchiveInterface.h"

#include "Codec/ImageEncoder.h"

namespace Mengine
{
	class ImageEncoderHTF
		: public ImageEncoder
	{
	public:
		ImageEncoderHTF();
		~ImageEncoderHTF();

	public:
		bool _initialize() override;

	public:
		size_t encode( const void * _buffer, size_t _size, const CodecDataInfo * _dataInfo ) override;

	protected:
		ArchivatorInterfacePtr m_archivator;
	};
}	
