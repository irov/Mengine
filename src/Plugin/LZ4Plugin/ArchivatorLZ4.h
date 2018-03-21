#pragma once

#include "Interface/ArchiveInterface.h"

#include "Core/ServantBase.h"

namespace Mengine
{
	class ArchivatorLZ4
		: public ServantBase<ArchivatorInterface>
	{
	public:
		ArchivatorLZ4();
		~ArchivatorLZ4() override;

    public:
        size_t compressBound( size_t _size ) const override;

    public:
		bool compress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _compressSize, EArchivatorCompress _compress ) override;
        bool decompress( void * _distance, size_t _bufferSize, const void * _source, size_t _sourceSize, size_t & _decompressSize ) override;
	};
}
