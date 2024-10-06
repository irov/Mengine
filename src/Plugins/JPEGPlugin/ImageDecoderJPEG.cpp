#include "ImageDecoderJPEG.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/FilePath.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/DebugFileHelper.h"
#include "Kernel/MemoryZero.h"

#include "Config/StdArg.h"
#include "Config/StdString.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( noreturn_t ) jpeg_error_exit( j_common_ptr _cinfo )
        {
            ImageDecoderJPEG * decoder = static_cast<ImageDecoderJPEG *>(_cinfo->client_data);

            return decoder->jpeg_error_exit( _cinfo );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( noreturn_t ) jpeg_output_message( j_common_ptr _cinfo )
        {
            ImageDecoderJPEG * decoder = static_cast<ImageDecoderJPEG *>(_cinfo->client_data);

            return decoder->jpeg_output_message( _cinfo );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) jpeg_init_source( j_decompress_ptr _cinfo )
        {
            ImageDecoderJPEG * decoder = static_cast<ImageDecoderJPEG *>(_cinfo->client_data);

            decoder->jpeg_init_source( _cinfo );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( boolean ) jpeg_fill_input_buffer( j_decompress_ptr _cinfo )
        {
            ImageDecoderJPEG * decoder = static_cast<ImageDecoderJPEG *>(_cinfo->client_data);

            return decoder->jpeg_fill_input_buffer( _cinfo );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) jpeg_skip_input_data( j_decompress_ptr _cinfo, long num_bytes )
        {
            ImageDecoderJPEG * decoder = static_cast<ImageDecoderJPEG *>(_cinfo->client_data);

            decoder->jpeg_skip_input_data( _cinfo, num_bytes );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) jpeg_term_source( j_decompress_ptr _cinfo )
        {
            ImageDecoderJPEG * decoder = static_cast<ImageDecoderJPEG *>(_cinfo->client_data);

            decoder->jpeg_term_source( _cinfo );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( int32_t ) jpeg_get_quality( const jpeg_decompress_struct * _jpegObject )
        {
            const JQUANT_TBL * tbl_ptrs = _jpegObject->quant_tbl_ptrs[0];

            if( tbl_ptrs == nullptr )
            {
                return 100;
            }

            UINT16 val1 = tbl_ptrs->quantval[0];
            UINT16 val2 = tbl_ptrs->quantval[1];

            val1 = (val1 * 100 - 50) / 16;
            val2 = (val2 * 100 - 50) / 11;

            if( val1 > 100 )
            {
                val1 = 5000 / val1;
            }
            else
            {
                val1 = (200 - val1) / 2;
            }

            if( val2 > 100 )
            {
                val2 = 5000 / val2;
            }
            else
            {
                val2 = (200 - val2) / 2;
            }

            return (val1 + val2) / 2;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderJPEG::ImageDecoderJPEG()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageDecoderJPEG::~ImageDecoderJPEG()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderJPEG::_initialize()
    {   
        Helper::memoryZeroPod( &m_errorMgr );

        jpeg_std_error( &m_errorMgr );

        m_errorMgr.error_exit = &Detail::jpeg_error_exit;
        m_errorMgr.output_message = &Detail::jpeg_output_message;

        Helper::memoryZeroPod( &m_decompressJpeg );

        jpeg_create_decompress( &m_decompressJpeg );
        
        m_decompressJpeg.err = &m_errorMgr;
        m_decompressJpeg.client_data = this;
        m_decompressJpeg.src = &m_sourceMgr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoderJPEG::_finalize()
    {
        jpeg_destroy_decompress( &m_decompressJpeg );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderJPEG::_prepareData()
    {
        if( MENGINE_JMP_SET( m_jmpBuffer ) != 0 )
        {
            LOGGER_ASSERTION( "jmp" );

            return false;
        }

        Helper::memoryZeroPod( &m_sourceMgr );

        m_sourceMgr.bytes_in_buffer = 0;
        m_sourceMgr.next_input_byte = nullptr;

        m_sourceMgr.init_source = &Detail::jpeg_init_source;
        m_sourceMgr.fill_input_buffer = &Detail::jpeg_fill_input_buffer;
        m_sourceMgr.skip_input_data = &Detail::jpeg_skip_input_data;
        m_sourceMgr.resync_to_restart = jpeg_resync_to_restart; // use default method 
        m_sourceMgr.term_source = &Detail::jpeg_term_source;

        jpeg_read_header( &m_decompressJpeg, TRUE );
        jpeg_calc_output_dimensions( &m_decompressJpeg );

        m_dataInfo.mipmaps = 1;
        m_dataInfo.width = m_decompressJpeg.image_width;
        m_dataInfo.height = m_decompressJpeg.image_height;
        m_dataInfo.quality = Detail::jpeg_get_quality( &m_decompressJpeg );

        switch( RGB_PIXELSIZE )
        {
        case 3:
            {
                m_dataInfo.format = PF_R8G8B8;
            }break;
        case 4:
            {
                m_dataInfo.format = PF_A8R8G8B8;
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderJPEG::_decode( const DecoderData * _decoderData )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _decoderData );
        MENGINE_ASSERTION_TYPE( _decoderData, const ImageDecoderData * );

        MENGINE_PROFILER_CATEGORY();

        if( MENGINE_JMP_SET( m_jmpBuffer ) != 0 )
        {
            LOGGER_ASSERTION( "jmp" );

            return 0;
        }

        const ImageDecoderData * decoderData = static_cast<const ImageDecoderData *>(_decoderData);

        void * buffer = decoderData->buffer;
        size_t size = decoderData->size;
        uint32_t flags = decoderData->flags;

        MENGINE_ASSERTION_FATAL( decoderData->size >= decoderData->pitch * m_dataInfo.height, "invalid buffer relation pitch to size bufferSize %zu - pitch %zu height %u"
            , decoderData->size
            , decoderData->pitch
            , m_dataInfo.height
        );

        jpeg_start_decompress( &m_decompressJpeg );

        uint32_t optionChannels = Helper::getPixelFormatChannels( decoderData->format );
        uint32_t dataChannels = Helper::getPixelFormatChannels( m_dataInfo.format );

        switch( flags & 0x0000ffff )
        {
        case DF_IMAGE_NONE:
            {
                if( optionChannels == dataChannels )
                {
                    JSAMPROW rgb_buffer = (JSAMPROW)buffer;

                    for( uint32_t j = 0; j != m_dataInfo.height; ++j )
                    {
                        jpeg_read_scanlines( &m_decompressJpeg, &rgb_buffer, 1 );

                        // Assume put_scanline_someplace wants a pointer and sample count.
                        rgb_buffer += decoderData->pitch;
                    }

#if RGB_PIXELSIZE == 4
                    if( (flags & DF_IMAGE_NOT_ADD_ALPHA) == 0 )
                    {
                        JSAMPROW alpha_buffer = (JSAMPROW)buffer;

                        for( uint32_t j = 0; j != m_dataInfo.height; ++j )
                        {
                            for( uint32_t i = 0; i != m_dataInfo.width; ++i )
                            {
                                alpha_buffer[i * 4 + 3] = 255; // alpha
                            }

                            alpha_buffer += decoderData->pitch;
                        }
                    }
#endif
                }
                else
                {
                    LOGGER_ERROR( "DEFAULT options channels %u != %u"
                        , optionChannels
                        , dataChannels
                    );

                    return 0;
                }
            }break;
        case DF_IMAGE_READ_ALPHA_ONLY:
            {
                if( optionChannels == 1 )
                {
                    StdString::memset( buffer, 255, size );

                    return size;
                }
                else
                {
                    LOGGER_ERROR( "DF_READ_ALPHA_ONLY options channels %u != 4"
                        , optionChannels
                    );

                    return 0;
                }
            }break;
        default:
            {
                LOGGER_ERROR( "unsupport options flag %u"
                    , decoderData->flags
                );

                return 0;
            }break;
        }

        m_decompressJpeg.output_scanline = m_decompressJpeg.output_height;
        jpeg_finish_decompress( &m_decompressJpeg );

        size_t readSize = decoderData->pitch * m_dataInfo.height;

        return readSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderJPEG::_rewind()
    {
        jpeg_destroy_decompress( &m_decompressJpeg );

        Helper::memoryZeroPod( &m_errorMgr );

        jpeg_std_error( &m_errorMgr );

        m_errorMgr.error_exit = &Detail::jpeg_error_exit;
        m_errorMgr.output_message = &Detail::jpeg_output_message;

        Helper::memoryZeroPod( &m_decompressJpeg );

        jpeg_create_decompress( &m_decompressJpeg );

        m_decompressJpeg.err = &m_errorMgr;
        m_decompressJpeg.client_data = this;
        m_decompressJpeg.src = &m_sourceMgr;

        const InputStreamInterfacePtr & stream = this->getStream();

        stream->rewind();

        if( this->_prepareData() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    noreturn_t ImageDecoderJPEG::jpeg_error_exit( j_common_ptr _cinfo )
    {
        Char buffer[JMSG_LENGTH_MAX + 1] = {'\0'};

        (*_cinfo->err->format_message)(_cinfo, buffer);

        LOGGER_ASSERTION( "jpeg '%s' error: %s"
            , Helper::getDebugFullPath( this->getStream() )
            , buffer
        );

        if( _cinfo->err->msg_parm.i[0] != 13 )
        {
            jpeg_destroy( _cinfo );

            MENGINE_JMP_JUMP( m_jmpBuffer, 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    noreturn_t ImageDecoderJPEG::jpeg_output_message( j_common_ptr _cinfo )
    {
        Char buffer[JMSG_LENGTH_MAX + 1] = {'\0'};

        (*_cinfo->err->format_message)(_cinfo, buffer);

        LOGGER_ASSERTION( "jpeg '%s' message: %s"
            , Helper::getDebugFullPath( this->getStream() )
            , buffer
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoderJPEG::jpeg_init_source( j_decompress_ptr _cinfo )
    {
        MENGINE_UNUSED( _cinfo );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    boolean ImageDecoderJPEG::jpeg_fill_input_buffer( j_decompress_ptr _cinfo )
    {
        const InputStreamInterfacePtr & stream = this->getStream();

        size_t nbytes = stream->read( m_JOCTETBuffer, MENGINE_DECODER_JPEG_INPUT_BUF_SIZE );

        if( nbytes <= 0 )
        {
            WARNMS( _cinfo, JWRN_JPEG_EOF );

            m_JOCTETBuffer[0] = (JOCTET)0xFF;
            m_JOCTETBuffer[1] = (JOCTET)JPEG_EOI;

            nbytes = 2;
        }

        m_sourceMgr.next_input_byte = m_JOCTETBuffer;
        m_sourceMgr.bytes_in_buffer = nbytes;

        return TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoderJPEG::jpeg_skip_input_data( j_decompress_ptr _cinfo, long num_bytes )
    {
        if( num_bytes <= 0 )
        {
            return;
        }

        while( num_bytes > (long)m_sourceMgr.bytes_in_buffer )
        {
            num_bytes -= (long)m_sourceMgr.bytes_in_buffer;

            boolean result = this->jpeg_fill_input_buffer( _cinfo );
            MENGINE_UNUSED( result );
        }

        m_sourceMgr.next_input_byte += (size_t)num_bytes;
        m_sourceMgr.bytes_in_buffer -= (size_t)num_bytes;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoderJPEG::jpeg_term_source( j_decompress_ptr _cinfo )
    {
        MENGINE_UNUSED( _cinfo );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
