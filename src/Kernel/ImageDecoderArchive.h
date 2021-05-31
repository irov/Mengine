#pragma once

#include "Interface/ArchivatorInterface.h"

#include "Kernel/ImageDecoder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ImageDecoderArchive
        : public ImageDecoder
    {
        DECLARE_FACTORABLE( ImageDecoderArchive );

    public:
        ImageDecoderArchive();
        ~ImageDecoderArchive() override;

    public:
        void setArchivator( const ArchivatorInterfacePtr & _archivator );
        const ArchivatorInterfacePtr & getArchivator() const;

        void setUncompressSize( size_t _size );
        size_t getUncompressSize() const;

    protected:
        size_t _decode( const DecoderData * _data ) override;

    protected:
        size_t decodeData_( void * const _buffer, size_t _bufferSize, size_t _pitch ) const;
        size_t decompressData_( const void * _source, size_t _sourceSize, void * const _dest, size_t _capacityDest, size_t _pitch ) const;

    protected:
        ArchivatorInterfacePtr m_archivator;

        size_t m_uncompressSize;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImageDecoderArchive> ImageDecoderArchivePtr;
    //////////////////////////////////////////////////////////////////////////
}
