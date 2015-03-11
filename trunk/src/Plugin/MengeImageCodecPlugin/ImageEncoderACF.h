#	pragma once

#   include "Interface/ArchiveInterface.h"

#	include "Codec/ImageEncoder.h"

namespace Menge
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
		size_t encode( const void * _buffer, const CodecDataInfo* _bufferDataInfo ) override;

	protected:
		ArchivatorInterfacePtr m_archivator;
	};
}	// namespace Menge
