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
        DECLARE_FACTORABLE( XmlToBinDecoder );

    public:
        XmlToBinDecoder();
        ~XmlToBinDecoder() override;

    public:
        bool initialize( const ThreadMutexInterfacePtr & _mutex ) override;
        void finalize() override;

    protected:
        const InputStreamInterfacePtr & getStream() const override;

    public:
        void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
        const XmlCodecDataInfo * getCodecDataInfo() const override;

    public:
        bool prepareData( const InputStreamInterfacePtr & _stream ) override;

    public:
        size_t decode( const DecoderData * _data ) override;
        bool rewind() override;

    public:
        bool seek( float _time ) override;
        float tell() const override;

    protected:
        ArchivatorInterfacePtr m_archivator;
        FileGroupInterfacePtr m_fileGroupDev;

        InputStreamInterfacePtr m_stream;
    };
}