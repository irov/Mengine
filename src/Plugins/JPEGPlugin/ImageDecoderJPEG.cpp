#include "ImageDecoderJPEG.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/FilePath.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"

#include "Config/StdArg.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_DECODER_JPEG_INPUT_BUF_SIZE
#define MENGINE_DECODER_JPEG_INPUT_BUF_SIZE 4096
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct DecoderJPEGSourceManager
        {
            jpeg_source_mgr base;

            InputStreamInterface * stream;
            JOCTET * buffer;
            boolean start_of_file;
        };
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( const FilePath & ) jpeg_get_debug_filepath( j_common_ptr _cinfo )
        {
            ImageDecoderJPEG * decoder = static_cast<ImageDecoderJPEG *>(_cinfo->client_data);

            const InputStreamInterfacePtr & stream = decoder->getStream();

            const FilePath & filePath = Helper::getInputStreamDebugFilePath( stream );

            return filePath;
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( noreturn_t ) jpeg_error_exit( j_common_ptr _cinfo )
        {
            Char buffer[JMSG_LENGTH_MAX] = {'\0'};

            (*_cinfo->err->format_message)(_cinfo, buffer);

            LOGGER_ERROR( "jpeg '%s' error: %s"
                , Detail::jpeg_get_debug_filepath( _cinfo ).c_str()
                , buffer
            );
        
            if( _cinfo->err->msg_parm.i[0] != 13 )
            {
                jpeg_destroy( _cinfo );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( noreturn_t ) jpeg_output_message( j_common_ptr _cinfo )
        {
            Char buffer[JMSG_LENGTH_MAX] = {'\0'};

            (*_cinfo->err->format_message)(_cinfo, buffer);

            LOGGER_ERROR( "jpeg '%s' message: %s"
                , Detail::jpeg_get_debug_filepath( _cinfo ).c_str()
                , buffer
            );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) jpeg_init_source( j_decompress_ptr cinfo )
        {
            DecoderJPEGSourceManager * src = (DecoderJPEGSourceManager *)cinfo->src;

            src->start_of_file = TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( boolean ) jpeg_fill_input_buffer( j_decompress_ptr cinfo )
        {
            DecoderJPEGSourceManager * src = (DecoderJPEGSourceManager *)cinfo->src;

            size_t nbytes = src->stream->read( src->buffer, MENGINE_DECODER_JPEG_INPUT_BUF_SIZE );

            if( nbytes <= 0 )
            {
                WARNMS( cinfo, JWRN_JPEG_EOF );

                src->buffer[0] = (JOCTET)0xFF;
                src->buffer[1] = (JOCTET)JPEG_EOI;

                nbytes = 2;
            }

            src->base.next_input_byte = src->buffer;
            src->base.bytes_in_buffer = nbytes;
            src->start_of_file = FALSE;

            return TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) jpeg_skip_input_data( j_decompress_ptr cinfo, long num_bytes )
        {
            DecoderJPEGSourceManager * src = (DecoderJPEGSourceManager *)cinfo->src;

            if( num_bytes > 0 )
            {
                while( num_bytes > (long)src->base.bytes_in_buffer )
                {
                    num_bytes -= (long)src->base.bytes_in_buffer;

                    boolean result = jpeg_fill_input_buffer( cinfo );
                    MENGINE_UNUSED( result );
                }

                src->base.next_input_byte += (size_t)num_bytes;
                src->base.bytes_in_buffer -= (size_t)num_bytes;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) jpeg_term_source( j_decompress_ptr cinfo )
        {
            MENGINE_UNUSED( cinfo );

            //Empty
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) jpeg_mengine_src( j_decompress_ptr cinfo, InputStreamInterface * _stream )
        {
            if( cinfo->src == nullptr )
            {
                DecoderJPEGSourceManager * manager = (DecoderJPEGSourceManager *)(*cinfo->mem->alloc_small)
                    ((j_common_ptr)cinfo, JPOOL_PERMANENT, SIZEOF( DecoderJPEGSourceManager ));

                manager->buffer = (JOCTET *)(*cinfo->mem->alloc_small)
                    ((j_common_ptr)cinfo, JPOOL_PERMANENT, MENGINE_DECODER_JPEG_INPUT_BUF_SIZE * SIZEOF( JOCTET ));

                cinfo->src = (struct jpeg_source_mgr *)manager;
            }

            DecoderJPEGSourceManager * src = (DecoderJPEGSourceManager *)cinfo->src;

            src->base.init_source = &jpeg_init_source;
            src->base.fill_input_buffer = &jpeg_fill_input_buffer;
            src->base.skip_input_data = &jpeg_skip_input_data;
            src->base.resync_to_restart = jpeg_resync_to_restart; // use default method 
            src->base.term_source = jpeg_term_source;
            src->base.bytes_in_buffer = 0;
            src->base.next_input_byte = nullptr;
            src->stream = _stream;
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
        // step 1: allocate and initialize JPEG decompression object
        m_jpegObject.err = jpeg_std_error( &m_errorMgr );
        m_jpegObject.client_data = this;

        m_errorMgr.error_exit = &Detail::jpeg_error_exit;
        m_errorMgr.output_message = &Detail::jpeg_output_message;

        jpeg_create_decompress( &m_jpegObject );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageDecoderJPEG::_finalize()
    {
        jpeg_destroy_decompress( &m_jpegObject );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderJPEG::_prepareData()
    {
        const InputStreamInterfacePtr & stream = this->getStream();

        Detail::jpeg_mengine_src( &m_jpegObject, stream.get() );

        jpeg_read_header( &m_jpegObject, TRUE );
        jpeg_calc_output_dimensions( &m_jpegObject );

        m_dataInfo.mipmaps = 1;
        m_dataInfo.width = m_jpegObject.image_width;
        m_dataInfo.height = m_jpegObject.image_height;
        m_dataInfo.quality = Detail::jpeg_get_quality( &m_jpegObject );

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

        const ImageDecoderData * decoderData = static_cast<const ImageDecoderData *>(_decoderData);

        void * buffer = decoderData->buffer;
        size_t size = decoderData->size;
        uint32_t flags = decoderData->flags;

        MENGINE_ASSERTION_FATAL( decoderData->size >= decoderData->pitch * m_dataInfo.height, "invalid buffer relation pitch to size bufferSize %zu - pitch %zu height %u"
            , decoderData->size
            , decoderData->pitch
            , m_dataInfo.height
        );

        jpeg_start_decompress( &m_jpegObject );

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
                        jpeg_read_scanlines( &m_jpegObject, &rgb_buffer, 1 );

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
                    MENGINE_MEMSET( buffer, 255, size );

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

        m_jpegObject.output_scanline = m_jpegObject.output_height;
        jpeg_finish_decompress( &m_jpegObject );

        size_t readSize = decoderData->pitch * m_dataInfo.height;

        return readSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderJPEG::_rewind()
    {
        jpeg_destroy_decompress( &m_jpegObject );

        m_jpegObject.err = jpeg_std_error( &m_errorMgr );
        m_jpegObject.client_data = this;

        m_errorMgr.error_exit = &Detail::jpeg_error_exit;
        m_errorMgr.output_message = &Detail::jpeg_output_message;

        jpeg_create_decompress( &m_jpegObject );

        const InputStreamInterfacePtr & stream = this->getStream();

        if( stream->seek( 0 ) == false )
        {
            return false;
        }

        if( this->_prepareData() == false )
        {
            return false;
        }

        return true;
    }
}
