#include "ImageConverterPVRToHTF.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Magic.h"
#include "Kernel/FilePath.h"
#include "Kernel/PathHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/ImageCodecHelper.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ImageConverterPVRToHTF::ImageConverterPVRToHTF()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ImageConverterPVRToHTF::~ImageConverterPVRToHTF()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ImageConverterPVRToHTF::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".htf" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageConverterPVRToHTF::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ImageConverterPVRToHTF::convert()
    {
        const FilePath & folderPath = m_options.fileGroup->getFolderPath();

        FilePath full_inputFilePath = Helper::concatenationFilePath( folderPath, m_options.inputFilePath );
        FilePath full_outputFilePath = Helper::concatenationFilePath( folderPath, m_options.outputFilePath );

        InputStreamInterfacePtr stream_intput = Helper::openInputStreamFile( m_fileGroup, full_inputFilePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_intput, "invalid open input file '%s'"
            , m_options.inputFilePath.c_str()
        );

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "pvrImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder for '%s'"
            , m_options.inputFilePath.c_str()
        );

        if( decoder->prepareData( stream_intput ) == false )
        {
            LOGGER_ERROR( "invalid prepare decoder for '%s'"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        uint32_t data_full_size = Helper::getImageCodecDataFullSize( dataInfo );

        MemoryInterfacePtr data_buffer = Helper::createMemoryCacheBuffer( data_full_size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data_buffer, "invalid cache memory %d for '%s'"
            , data_full_size
            , m_options.inputFilePath.c_str()
        );

        void * data_memory = data_buffer->getBuffer();

        uint8_t * miplevel_data_memory = reinterpret_cast<uint8_t *>(data_memory);

        for( uint32_t i = 0; i != dataInfo->mipmaps; ++i )
        {
            ImageCodecOptions decoder_options;

            uint32_t miplevel_data_size = Helper::getImageCodecDataMipMapSize( dataInfo, i );

            decoder_options.pitch = miplevel_data_size / (dataInfo->height >> i);
            decoder_options.channels = 3;

            if( decoder->setOptions( &decoder_options ) == false )
            {
                LOGGER_ERROR( "invalid optionize decoder for '%s'"
                    , m_options.inputFilePath.c_str()
                );

                return false;
            }

            if( decoder->rewind() == false )
            {
                LOGGER_ERROR( "invalid rewind '%s'"
                    , m_options.inputFilePath.c_str()
                );

                return false;
            }

            DecoderData data;
            data.buffer = miplevel_data_memory;
            data.size = miplevel_data_size;

            if( decoder->decode( &data ) == 0 )
            {
                LOGGER_ERROR( "invalid decode '%s'"
                    , m_options.inputFilePath.c_str()
                );

                return false;
            }

            miplevel_data_memory += miplevel_data_size;
        }

        OutputStreamInterfacePtr stream_output = Helper::openOutputStreamFile( m_fileGroup, full_outputFilePath, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_output, "invalid open output '%s' for file '%s'"
            , full_outputFilePath.c_str()
            , m_options.inputFilePath.c_str()
        );

        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
            ->createEncoderT<ImageEncoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "htfImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( encoder, "invalid create encoder 'htfImage'" );

        if( encoder->initialize( stream_output ) == false )
        {
            LOGGER_ERROR( "invalid initialize encoder '%s'"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        ImageCodecOptions encoder_options;
        encoder_options.channels = 3;
        encoder_options.pitch = dataInfo->width;

        if( encoder->setOptions( &encoder_options ) == false )
        {
            LOGGER_ERROR( "invalid optionize encoder '%s'"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        ImageCodecDataInfo htfDataInfo;
        htfDataInfo.width = dataInfo->width;
        htfDataInfo.height = dataInfo->height;
        htfDataInfo.mipmaps = dataInfo->mipmaps;
        htfDataInfo.format = dataInfo->format;

        size_t encode_byte = encoder->encode( data_memory, data_full_size, &htfDataInfo );

        if( encode_byte == 0 )
        {
            LOGGER_ERROR( "'%s' invalid encode"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}