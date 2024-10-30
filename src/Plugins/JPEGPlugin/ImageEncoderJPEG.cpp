#include "ImageEncoderJPEG.h"

#include "Kernel/Logger.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionType.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/MemoryZero.h"
#include "Kernel/AllocatorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( noreturn_t ) jpeg_error_exit( j_common_ptr _cinfo )
        {
            ImageEncoderJPEG * encoder = (ImageEncoderJPEG *)_cinfo->client_data;

            encoder->jpeg_error_exit( _cinfo );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) jpeg_output_message( j_common_ptr _cinfo )
        {
            ImageEncoderJPEG * encoder = (ImageEncoderJPEG *)_cinfo->client_data;

            encoder->jpeg_output_message( _cinfo );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) jpeg_init_destination( j_compress_ptr _cinfo )
        {
            ImageEncoderJPEG * encoder = (ImageEncoderJPEG *)_cinfo->client_data;

            encoder->jpeg_init_destination( _cinfo );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( boolean ) jpeg_empty_output_buffer( j_compress_ptr _cinfo )
        {
            ImageEncoderJPEG * encoder = (ImageEncoderJPEG *)_cinfo->client_data;

            return encoder->jpeg_empty_output_buffer( _cinfo );
        }
        //////////////////////////////////////////////////////////////////////////
        METHODDEF( void ) jpeg_term_destination( j_compress_ptr _cinfo )
        {
            ImageEncoderJPEG * encoder = (ImageEncoderJPEG *)_cinfo->client_data;

            encoder->jpeg_term_destination( _cinfo );
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
    bool ImageEncoderJPEG::_initialize()
    {
        MENGINE_JMPBUF_ALLOCATE( m_jmpBuffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageEncoderJPEG::_finalize()
    {
        MENGINE_JMPBUF_DEALLOCATE( m_jmpBuffer );        
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ImageEncoderJPEG::encode( const EncoderData * _encoderData, const CodecDataInfo * _dataInfo )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _encoderData, "invalid encode data" );
        MENGINE_ASSERTION_TYPE( _encoderData, const ImageEncoderData *, "invalid encode data" );

        MENGINE_ASSERTION_MEMORY_PANIC( _dataInfo, "invalid encode data" );
        MENGINE_ASSERTION_TYPE( _dataInfo, const ImageCodecDataInfo *, "invalid encode data" );

        if( MENGINE_JMP_SET( m_jmpBuffer->value ) != 0 )
        {
            return 0;
        }

        const ImageCodecDataInfo * dataInfo = static_cast<const ImageCodecDataInfo *>(_dataInfo);

        Helper::memoryZeroPod( &m_errorMgr );

        jpeg_std_error( &m_errorMgr );

        m_errorMgr.error_exit = &Detail::jpeg_error_exit;
        m_errorMgr.output_message = &Detail::jpeg_output_message;

        Helper::memoryZeroPod( &m_compressJpeg );

        m_compressJpeg.err = &m_errorMgr;
        m_compressJpeg.client_data = this;

        jpeg_create_compress( &m_compressJpeg );

        Helper::memoryZeroPod( &m_destinationMgr );

        m_destinationMgr.next_output_byte = nullptr;
        m_destinationMgr.free_in_buffer = 0;

        m_destinationMgr.init_destination = &Detail::jpeg_init_destination;
        m_destinationMgr.empty_output_buffer = &Detail::jpeg_empty_output_buffer;
        m_destinationMgr.term_destination = &Detail::jpeg_term_destination;

        m_compressJpeg.dest = &m_destinationMgr;

        m_compressJpeg.image_width = (JDIMENSION)dataInfo->width;
        m_compressJpeg.image_height = (JDIMENSION)dataInfo->height;
        m_compressJpeg.input_components = Helper::getPixelFormatChannels( dataInfo->format );

        m_compressJpeg.in_color_space = JCS_RGB;

        jpeg_set_defaults( &m_compressJpeg );
        jpeg_set_quality( &m_compressJpeg, dataInfo->quality, TRUE );

        jpeg_start_compress( &m_compressJpeg, TRUE );

        const ImageEncoderData * encoderData = static_cast<const ImageEncoderData *>(_encoderData);

        JSAMPROW jpeg_buffer = (JSAMPROW)encoderData->buffer;

        size_t pitch = encoderData->pitch;

        JSAMPROW row_pointer[1];
        while( m_compressJpeg.next_scanline < m_compressJpeg.image_height )
        {
            row_pointer[0] = jpeg_buffer + m_compressJpeg.next_scanline * pitch;
            jpeg_write_scanlines( &m_compressJpeg, row_pointer, 1 );
        }

        jpeg_finish_compress( &m_compressJpeg );
        jpeg_destroy_compress( &m_compressJpeg );

        size_t size = pitch * dataInfo->height;

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    noreturn_t ImageEncoderJPEG::jpeg_error_exit( j_common_ptr _cinfo )
    {
        Char buffer[JMSG_LENGTH_MAX + 1] = {'\0'};

        (*m_errorMgr.format_message)(_cinfo, buffer);

        LOGGER_ASSERTION( "%s"
            , buffer
        );

        if( m_errorMgr.msg_parm.i[0] != 13 )
        {
            jpeg_destroy( _cinfo );

            MENGINE_JMP_JUMP( m_jmpBuffer->value, 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageEncoderJPEG::jpeg_output_message( j_common_ptr _cinfo )
    {
        Char buffer[JMSG_LENGTH_MAX + 1] = {'\0'};

        (*m_errorMgr.format_message)(_cinfo, buffer);

        LOGGER_ASSERTION( "%s"
            , buffer
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageEncoderJPEG::jpeg_init_destination( j_compress_ptr _cinfo )
    {
        MENGINE_UNUSED( _cinfo );

        m_destinationMgr.next_output_byte = m_JOCTETBuffer;
        m_destinationMgr.free_in_buffer = MENGINE_JPEG_OUTPUT_BUF_SIZE;
    }
    //////////////////////////////////////////////////////////////////////////
    boolean ImageEncoderJPEG::jpeg_empty_output_buffer( j_compress_ptr _cinfo )
    {
        MENGINE_UNUSED( _cinfo );

        const OutputStreamInterfacePtr & stream = this->getStream();

        stream->write( m_JOCTETBuffer, MENGINE_JPEG_OUTPUT_BUF_SIZE );

        m_destinationMgr.next_output_byte = m_JOCTETBuffer;
        m_destinationMgr.free_in_buffer = MENGINE_JPEG_OUTPUT_BUF_SIZE;

        return TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageEncoderJPEG::jpeg_term_destination( j_compress_ptr _cinfo )
    {
        MENGINE_UNUSED( _cinfo );

        size_t data_count = MENGINE_JPEG_OUTPUT_BUF_SIZE - m_destinationMgr.free_in_buffer;

        if( data_count <= 0 )
        {
            return;
        }
        
        const OutputStreamInterfacePtr & stream = this->getStream();

        stream->write( m_JOCTETBuffer, data_count );
    }
    //////////////////////////////////////////////////////////////////////////
}
