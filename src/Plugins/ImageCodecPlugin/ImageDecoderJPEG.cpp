#include "ImageDecoderJPEG.h"

#include "Interface/AllocatorServiceInterface.h"

#include "Kernel/Logger.h"

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
        METHODDEF( noreturn_t ) s_jpegErrorExit( j_common_ptr _cinfo )
        {
            {
                Char buffer[JMSG_LENGTH_MAX] = {0};

                (*_cinfo->err->format_message)(_cinfo, buffer);

                LOGGER_ERROR( "jpeg error: %s"
                    , buffer
                );
            }

            if( _cinfo->err->msg_parm.i[0] != 13 )
            {
                jpeg_destroy( _cinfo );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( noreturn_t ) s_jpegOutputMessage( j_common_ptr _cinfo )
        {
            char buffer[JMSG_LENGTH_MAX] = {0};

            (*_cinfo->err->format_message)(_cinfo, buffer);

            LOGGER_ERROR( "jpeg message: %s"
                , buffer
            );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) s_init_source( j_decompress_ptr cinfo )
        {
            DecoderJPEGSourceManager * src = (DecoderJPEGSourceManager *)cinfo->src;

            src->start_of_file = TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( boolean ) s_fill_input_buffer( j_decompress_ptr cinfo )
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
        METHODDEF( void ) s_skip_input_data( j_decompress_ptr cinfo, long num_bytes )
        {
            DecoderJPEGSourceManager * src = (DecoderJPEGSourceManager *)cinfo->src;

            if( num_bytes > 0 )
            {
                while( num_bytes > (long)src->base.bytes_in_buffer )
                {
                    num_bytes -= (long)src->base.bytes_in_buffer;

                    boolean result = s_fill_input_buffer( cinfo );
                    MENGINE_UNUSED( result );
                }

                src->base.next_input_byte += (size_t)num_bytes;
                src->base.bytes_in_buffer -= (size_t)num_bytes;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) s_term_source( j_decompress_ptr cinfo )
        {
            MENGINE_UNUSED( cinfo );

            //Empty
        }
        //////////////////////////////////////////////////////////////////////////
        GLOBAL( void ) s_jpeg_mengine_src( j_decompress_ptr cinfo, InputStreamInterface * _stream )
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

            src->base.init_source = &s_init_source;
            src->base.fill_input_buffer = &s_fill_input_buffer;
            src->base.skip_input_data = &s_skip_input_data;
            src->base.resync_to_restart = jpeg_resync_to_restart; // use default method 
            src->base.term_source = s_term_source;
            src->base.bytes_in_buffer = 0;
            src->base.next_input_byte = nullptr;
            src->stream = _stream;
        }
        //////////////////////////////////////////////////////////////////////////
        static int32_t s_getQuality( const jpeg_decompress_struct * _jpegObject )
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
        static void * my_jpeg_malloc( size_t _size, void * _ud )
        {
            MENGINE_UNUSED( _ud );

            void * p = ALLOCATOR_SERVICE()
                ->malloc( _size, "jpeg" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void my_jpeg_free( void * _ptr, void * _ud )
        {
            MENGINE_UNUSED( _ud );

            ALLOCATOR_SERVICE()
                ->free( _ptr, "jpeg" );
        }
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
        set_jpeg_allocator( &Detail::my_jpeg_malloc, &Detail::my_jpeg_free, nullptr );

        // step 1: allocate and initialize JPEG decompression object
        m_jpegObject.err = jpeg_std_error( &m_errorMgr );
        m_jpegObject.client_data = this;

        m_errorMgr.error_exit = &Detail::s_jpegErrorExit;
        m_errorMgr.output_message = &Detail::s_jpegOutputMessage;

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
        // step 2a: specify data source (eg, a handle)
        Detail::s_jpeg_mengine_src( &m_jpegObject, m_stream.get() );

        // step 3: read handle parameters with jpeg_read_header()
        jpeg_read_header( &m_jpegObject, TRUE );

        jpeg_calc_output_dimensions( &m_jpegObject );

        m_dataInfo.depth = 1;
        m_dataInfo.mipmaps = 1;
        m_dataInfo.width = m_jpegObject.image_width;
        m_dataInfo.height = m_jpegObject.image_height;
        m_dataInfo.quality = Detail::s_getQuality( &m_jpegObject );

        m_dataInfo.channels = RGB_PIXELSIZE;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageDecoderJPEG::_decode( void * _buffer, size_t _bufferSize )
    {
        if( _bufferSize < m_options.pitch * m_dataInfo.height )
        {
            LOGGER_ERROR( "invalid buffer relation pitch to size bufferSize %zu - pitch %zu height %u"
                , _bufferSize
                , m_options.pitch
                , m_dataInfo.height
            );

            return 0;
        }

        jpeg_start_decompress( &m_jpegObject );

        if( m_options.flags == 0 )
        {
            if( m_options.channels == m_dataInfo.channels )
            {
                JSAMPROW rgb_buffer = (JSAMPROW)_buffer;

                for( uint32_t j = 0; j != m_dataInfo.height; ++j )
                {
                    jpeg_read_scanlines( &m_jpegObject, &rgb_buffer, 1 );

                    // Assume put_scanline_someplace wants a pointer and sample count.
                    rgb_buffer += m_options.pitch;
                }

#if RGB_PIXELSIZE == 4
                if( (m_options.flags & DF_NOT_ADD_ALPHA) == 0 )
                {
                    JSAMPROW alpha_buffer = (JSAMPROW)_buffer;
                    for( uint32_t j = 0; j != m_dataInfo.height; ++j )
                    {
                        for( uint32_t i = 0; i != m_dataInfo.width; ++i )
                        {
                            alpha_buffer[i * 4 + 3] = 255; // alpha
                        }

                        alpha_buffer += m_options.pitch;
                    }
                }
#endif
            }
            else
            {
                LOGGER_ERROR( "DEFAULT options channels %d != %d"
                    , m_options.channels
                    , m_dataInfo.channels
                );

                return 0;
            }
        }
        else if( m_options.flags & DF_READ_ALPHA_ONLY )
        {
            if( m_options.channels == 1 )
            {
                MENGINE_MEMSET( _buffer, 255, _bufferSize );

                return _bufferSize;
            }
            else
            {
                LOGGER_ERROR( "DF_READ_ALPHA_ONLY options channels %d != 4"
                    , m_options.channels
                );

                return 0;
            }
        }

        m_jpegObject.output_scanline = m_jpegObject.output_height;
        jpeg_finish_decompress( &m_jpegObject );

        size_t readSize = m_options.pitch * m_dataInfo.height;

        return readSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImageDecoderJPEG::_rewind()
    {
        jpeg_destroy_decompress( &m_jpegObject );

        m_jpegObject.err = jpeg_std_error( &m_errorMgr );
        m_jpegObject.client_data = this;

        m_errorMgr.error_exit = &Detail::s_jpegErrorExit;
        m_errorMgr.output_message = &Detail::s_jpegOutputMessage;

        jpeg_create_decompress( &m_jpegObject );

        if( m_stream->seek( 0 ) == false )
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
