#pragma once

#include "JPEGIncluder.h"

#include "Kernel/ImageDecoder.h"

#include "Config/StdSetJMP.h"

#ifndef MENGINE_DECODER_JPEG_INPUT_BUF_SIZE
#define MENGINE_DECODER_JPEG_INPUT_BUF_SIZE 4096
#endif

namespace Mengine
{
    class ImageDecoderJPEG
        : public ImageDecoder
    {
        DECLARE_FACTORABLE( ImageDecoderJPEG );

    public:
        ImageDecoderJPEG();
        ~ImageDecoderJPEG() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        bool _prepareData() override;

    protected:
        size_t _decode( const DecoderData * _data ) override;

    protected:
        bool _rewind() override;

    public:
        noreturn_t jpeg_error_exit( j_common_ptr _cinfo );
        noreturn_t jpeg_output_message( j_common_ptr _cinfo );
        void jpeg_init_source( j_decompress_ptr _cinfo );
        boolean jpeg_fill_input_buffer( j_decompress_ptr _cinfo );
        void jpeg_skip_input_data( j_decompress_ptr _cinfo, long num_bytes );
        void jpeg_term_source( j_decompress_ptr cinfo );
        
    protected:
        jpeg_decompress_struct m_decompressJpeg;
        jpeg_source_mgr m_sourceMgr;
        jpeg_error_mgr m_errorMgr;
        ;
        JOCTET m_JOCTETBuffer[MENGINE_DECODER_JPEG_INPUT_BUF_SIZE];

        MENGINE_JMP_DECLARE( m_jmpBuffer );
    };
}
