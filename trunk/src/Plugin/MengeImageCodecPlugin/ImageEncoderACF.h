#	pragma once

#   include "Interface/ArchiveInterface.h"

#	include "Codec/PickEncoder.h"

namespace Menge
{
	class ImageEncoderACF
		: public PickEncoder
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
