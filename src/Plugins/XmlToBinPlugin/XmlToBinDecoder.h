#pragma once

#include "Interface/ArchivatorInterface.h"
#include "Interface/FileGroupInterface.h"

#include "XmlToBinInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class XmlToBinDecoder
        : public XmlDecoderInterface
        , public Factorable
    {
    public:
        XmlToBinDecoder();
        ~XmlToBinDecoder() override;

    public:
        bool initialize() override;
        void finalize() override;

    protected:
        const InputStreamInterfacePtr & getStream() const override;

    public:
        bool setOptions( const CodecOptions * _options ) override;

    public:
        void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
        const XmlCodecDataInfo * getCodecDataInfo() const override;

    public:
        bool prepareData( const InputStreamInterfacePtr & _stream ) override;

    public:
        size_t decode( void * _buffer, size_t _bufferSize ) override;
        bool rewind() override;

    public:
        bool seek( float _timing ) override;
        float tell() const override;

    protected:
        ArchivatorInterfacePtr m_archivator;

        FileGroupInterfacePtr m_fileGroupDev;

        InputStreamInterfacePtr m_stream;

        XmlCodecOptions m_options;
    };
}