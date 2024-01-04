#include "ImageConverterDDSToHTF.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileServiceInterface.h"
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
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ImageConverterDDSToHTF::ImageConverterDDSToHTF()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ImageConverterDDSToHTF::~ImageConverterDDSToHTF()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ImageConverterDDSToHTF::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".htf" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageConverterDDSToHTF::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ImageConverterDDSToHTF::convert()
    {
        const FileGroupInterfacePtr & inputFileGroup = m_options.inputContent->getFileGroup();
        const FilePath & inputFilePath = m_options.inputContent->getFilePath();

        const FileGroupInterfacePtr & outputFileGroup = m_options.outputContent->getFileGroup();
        const FilePath & outputFilePath = m_options.outputContent->getFilePath();

        const FilePath & inputFolderPath = inputFileGroup->getFolderPath();
        const FilePath & outputFolderPath = outputFileGroup->getFolderPath();

        FilePath full_inputFilePath = Helper::concatenationFilePath( inputFolderPath, inputFilePath );
        FilePath full_outputFilePath = Helper::concatenationFilePath( outputFolderPath, outputFilePath );

        InputStreamInterfacePtr stream_intput = Helper::openInputStreamFile( m_devFileGroup, full_inputFilePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_intput, "invalid open input file '%s'"
            , Helper::getContentFullPath( m_options.inputContent )
        );

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( STRINGIZE_STRING_LOCAL( "ddsImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder for '%s'"
            , Helper::getContentFullPath( m_options.inputContent )
        );

        if( decoder->prepareData( stream_intput ) == false )
        {
            LOGGER_ERROR( "invalid prepare decoder for '%s'"
                , Helper::getContentFullPath( m_options.inputContent )
            );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        uint32_t data_full_size = Helper::getImageCodecDataFullSize( dataInfo );

        MemoryInterfacePtr data_buffer = Helper::createMemoryCacheBuffer( data_full_size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data_buffer, "invalid cache memory [%u] for '%s'"
            , data_full_size
            , Helper::getContentFullPath( m_options.inputContent )
        );

        void * data_memory = data_buffer->getBuffer();

        uint8_t * miplevel_data_memory = reinterpret_cast<uint8_t *>(data_memory);

        for( uint32_t i = 0; i != dataInfo->mipmaps; ++i )
        {
            if( decoder->rewind() == false )
            {
                LOGGER_ERROR( "invalid rewind '%s'"
                    , Helper::getContentFullPath( m_options.inputContent )
                );

                return false;
            }

            uint32_t miplevel_data_size = Helper::getImageCodecDataMipMapSize( dataInfo, i );

            ImageDecoderData data;
            data.buffer = miplevel_data_memory;
            data.size = miplevel_data_size;
            data.pitch = miplevel_data_size / (dataInfo->height >> i);
            data.format = PF_R8G8B8;

            if( decoder->decode( &data ) == 0 )
            {
                LOGGER_ERROR( "invalid decode '%s'"
                    , Helper::getContentFullPath( m_options.inputContent )
                );

                return false;
            }

            miplevel_data_memory += miplevel_data_size;
        }

        OutputStreamInterfacePtr stream_output = Helper::openOutputStreamFile( m_devFileGroup, full_outputFilePath, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_output, "invalid open output '%s' for '%s'"
            , full_outputFilePath.c_str()
            , Helper::getContentFullPath( m_options.inputContent )
        );

        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
            ->createEncoder( STRINGIZE_STRING_LOCAL( "htfImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( encoder, "invalid create encoder 'htfImage'" );

        if( encoder->initialize( stream_output ) == false )
        {
            LOGGER_ERROR( "'%s' invalid initialize encoder"
                , Helper::getContentFullPath( m_options.inputContent )
            );

            return false;
        }

        ImageEncoderData htfEncoderData;
        htfEncoderData.buffer = data_memory;
        htfEncoderData.size = data_full_size;
        htfEncoderData.pitch = dataInfo->width;

        ImageCodecDataInfo htfDataInfo;
        htfDataInfo.width = dataInfo->width;
        htfDataInfo.height = dataInfo->height;
        htfDataInfo.mipmaps = dataInfo->mipmaps;
        htfDataInfo.format = dataInfo->format;

        size_t encode_byte = encoder->encode( &htfEncoderData, &htfDataInfo );

        encoder->finalize();

        if( Helper::closeOutputStreamFile( m_devFileGroup, stream_output ) == false )
        {
            LOGGER_ERROR( "'%s' invalid close output '%s'"
                , Helper::getContentFullPath( m_options.inputContent )
                , full_outputFilePath.c_str()
            );

            return false;
        }

        if( encode_byte == 0 )
        {
            LOGGER_ERROR( "'%s' invalid encode"
                , Helper::getContentFullPath( m_options.inputContent )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}