#pragma once

#include "Interface/ImageCodecInterface.h"

#include "Kernel/Decoder.h"

namespace Mengine
{
    class ImageDecoder
        : public Decoder<ImageDecoderInterface>
    {
    public:
        ImageDecoder();
        ~ImageDecoder() override;

    public:
        bool setOptions( const CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
        const ImageCodecDataInfo * getCodecDataInfo() const override;

    protected:
        void sweezleAlpha1( uint32_t _width, uint32_t _height, void * const _buffer, size_t _pitch );
        void sweezleAlpha3( uint32_t _width, uint32_t _height, void * const _buffer, size_t _pitch );

    protected:
        ImageCodecOptions m_options;
        ImageCodecDataInfo m_dataInfo;
    };
}
