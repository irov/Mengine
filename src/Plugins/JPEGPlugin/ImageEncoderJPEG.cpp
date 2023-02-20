#include "ImageEncoderJPEG.h"

#include "Kernel/Logger.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/PixelFormatHelper.h"

#ifndef MENGINE_JPEG_OUTPUT_BUF_SIZE
#define MENGINE_JPEG_OUTPUT_BUF_SIZE 4096
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct DestinationManager
        {
            struct jpeg_destination_mgr pub;

            OutputStreamInterface * m_stream;
            JOCTET * buffer;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef DestinationManager * mengine_dst_ptr;
        //////////////////////////////////////////////////////////////////////////
        struct EncoderJPEGErrorManager
        {
            jpeg_error_mgr pub;
#if !defined(MENGINE_SETJMP_UNSUPPORTED)
            jmp_buf setjmp_buffer;
#endif
        };
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) s_jpegErrorExit( j_common_ptr _cinfo )
        {
            EncoderJPEGErrorManager * mErr = (EncoderJPEGErrorManager *)_cinfo->err;

            Char buffer[JMSG_LENGTH_MAX] = {'\0'};

            (*mErr->pub.format_message)(_cinfo, buffer);

            LOGGER_ERROR( "%s"
                , buffer
            );

            if( mErr->pub.msg_parm.i[0] != 13 )
            {
                jpeg_destroy( _cinfo );

#if !defined(MENGINE_SETJMP_UNSUPPORTED)
                longjmp( mErr->setjmp_buffer, 1 );
#endif
            }
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) s_jpegOutputMessage( j_common_ptr _cinfo )
        {
            EncoderJPEGErrorManager * mErr = (EncoderJPEGErrorManager *)_cinfo->err;

            Char buffer[JMSG_LENGTH_MAX] = {'\0'};

            (*mErr->pub.format_message)(_cinfo, buffer);

            LOGGER_ERROR( "%s"
                , buffer
            );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) init_destination( j_compress_ptr _cinfo )
        {
            mengine_dst_ptr dest = (mengine_dst_ptr)_cinfo->dest;

            dest->buffer = (JOCTET *)
                (*_cinfo->mem->alloc_small) ((j_common_ptr)_cinfo, JPOOL_IMAGE,
                    MENGINE_JPEG_OUTPUT_BUF_SIZE * sizeof( uint8_t ));

            dest->pub.next_output_byte = dest->buffer;
            dest->pub.free_in_buffer = MENGINE_JPEG_OUTPUT_BUF_SIZE;
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( boolean ) empty_output_buffer( j_compress_ptr _cinfo )
        {
            mengine_dst_ptr dest = (mengine_dst_ptr)_cinfo->dest;

            dest->m_stream->write( dest->buffer, MENGINE_JPEG_OUTPUT_BUF_SIZE );

            dest->pub.next_output_byte = dest->buffer;
            dest->pub.free_in_buffer = MENGINE_JPEG_OUTPUT_BUF_SIZE;

            return TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) term_destination( j_compress_ptr _cinfo )
        {
            mengine_dst_ptr dest = (mengine_dst_ptr)_cinfo->dest;

            size_t datacount = MENGINE_JPEG_OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

            if( datacount > 0 )
            {
                dest->m_stream->write( dest->buffer, datacount );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static void jpeg_mengine_dst( j_compress_ptr _cinfo, OutputStreamInterface * _stream )
        {
            mengine_dst_ptr dest;

            if( _cinfo->dest == nullptr )
            {
                _cinfo->dest = (struct jpeg_destination_mgr *)(*_cinfo->mem->alloc_small)
                    ((j_common_ptr)_cinfo, JPOOL_PERMANENT, sizeof( DestinationManager ));
            }

            dest = (mengine_dst_ptr)_cinfo->dest;
            dest->pub.init_destination = &Detail::init_destination;
            dest->pub.empty_output_buffer = &Detail::empty_output_buffer;
            dest->pub.term_destination = &Detail::term_destination;
            dest->m_stream = _stream;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ImageEncoderJPEG::ImageEncoderJPEG()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImageEncoderJPEG::~ImageEncoderJPEG()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageEncoderJPEG::encode( const EncoderData * _encoderData, const CodecDataInfo * _dataInfo )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _encoderData );
        MENGINE_ASSERTION_TYPE( _encoderData, const ImageEncoderData * );        

        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo );
        MENGINE_ASSERTION_TYPE( _dataInfo, const ImageCodecDataInfo * );

        const ImageCodecDataInfo * dataInfo = static_cast<const ImageCodecDataInfo *>(_dataInfo);

        Detail::EncoderJPEGErrorManager errorMgr;
        errorMgr.pub.error_exit = &Detail::s_jpegErrorExit;
        errorMgr.pub.output_message = &Detail::s_jpegOutputMessage;

#if !defined(MENGINE_SETJMP_UNSUPPORTED)
#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#endif 
        if( setjmp( errorMgr.setjmp_buffer ) )
        {
            LOGGER_ERROR( "jmp" );

            return 0;
        }
#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#endif
#endif

        struct jpeg_compress_struct cinfo = {0};
        cinfo.err = jpeg_std_error( &errorMgr.pub );
        cinfo.client_data = nullptr;

        jpeg_create_compress( &cinfo );

        Detail::jpeg_mengine_dst( &cinfo, m_stream.get() );

        cinfo.image_width = (JDIMENSION)dataInfo->width;
        cinfo.image_height = (JDIMENSION)dataInfo->height;
        cinfo.input_components = Helper::getPixelFormatChannels( dataInfo->format );

        cinfo.in_color_space = JCS_RGB;

        jpeg_set_defaults( &cinfo );
        jpeg_set_quality( &cinfo, dataInfo->quality, TRUE );

        jpeg_start_compress( &cinfo, TRUE );

        const ImageEncoderData * encoderData = static_cast<const ImageEncoderData *>(_encoderData);

        JSAMPROW jpeg_buffer = (JSAMPROW)encoderData->buffer;

        size_t pitch = encoderData->pitch;

        JSAMPROW row_pointer[1];
        while( cinfo.next_scanline < cinfo.image_height )
        {
            row_pointer[0] = jpeg_buffer + cinfo.next_scanline * pitch;
            jpeg_write_scanlines( &cinfo, row_pointer, 1 );
        }

        jpeg_finish_compress( &cinfo );
        jpeg_destroy_compress( &cinfo );

        size_t size = pitch * dataInfo->height;

        return size;
    }
}
