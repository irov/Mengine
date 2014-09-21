#	pragma once

#   include "Interface/ArchiveInterface.h"

#	include "Codec/PickEncoder.h"

namespace Menge
{
	class ImageEncoderHTF
		: public PickEncoder
	{
	public:
		ImageEncoderHTF();
		~ImageEncoderHTF();

	public:
		bool _initialize() override;

	public:
		size_t encode( const void * _buffer, const CodecDataInfo * _dataInfo ) override;

	protected:
		ArchivatorInterfacePtr m_archivator;
	};
}	// namespace Menge
