#pragma once

#include "JPEGIncluder.h"

#include "Kernel/ImageEncoder.h"

#include "Config/StdSetJMP.h"

#ifndef MENGINE_JPEG_OUTPUT_BUF_SIZE
#define MENGINE_JPEG_OUTPUT_BUF_SIZE 4096
#endif

namespace Mengine
{
    class ImageEncoderJPEG
        : public ImageEncoder
    {
        DECLARE_FACTORABLE( ImageEncoderJPEG );

    public:
        ImageEncoderJPEG();
        ~ImageEncoderJPEG() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    public:
        size_t encode( const EncoderData * _encoderData, const CodecDataInfo * _dataInfo ) override;

    public:
        noreturn_t jpeg_error_exit( j_common_ptr _cinfo );
        void jpeg_output_message( j_common_ptr _cinfo );
        void jpeg_init_destination( j_compress_ptr _cinfo );
        boolean jpeg_empty_output_buffer( j_compress_ptr _cinfo );
        void jpeg_term_destination( j_compress_ptr _cinfo );

    protected:
        jpeg_compress_struct m_compressJpeg;
        jpeg_destination_mgr m_destinationMgr;
        jpeg_error_mgr m_errorMgr;

        JOCTET m_JOCTETBuffer[MENGINE_JPEG_OUTPUT_BUF_SIZE];

        MENGINE_JMPBUF_DECLARE( m_jmpBuffer );
    };
}