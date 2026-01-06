#include "ImageConverterPVRToHTF.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/Logger.h"
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
        const FileGroupInterfacePtr & inputFileGroup = m_options.inputContent->getFileGroup();
        const FilePath & inputFilePath = m_options.inputContent->getFilePath();

        const FileGroupInterfacePtr & outputFileGroup = m_options.outputContent->getFileGroup();
        const FilePath & outputFilePath = m_options.outputContent->getFilePath();

        const FilePath & inputFolderPath = inputFileGroup->getFolderPath();
        const FilePath & outputFolderPath = outputFileGroup->getFolderPath();

        FilePath full_inputFilePath = Helper::concatenateFilePath( {inputFolderPath, inputFilePath} );
        FilePath full_outputFilePath = Helper::concatenateFilePath( {outputFolderPath, outputFilePath} );

        ContentInterfacePtr content_input = Helper::makeFileContent( m_fileGroupDev, full_inputFilePath, MENGINE_DOCUMENT_FACTORABLE );

        InputStreamInterfacePtr stream_input = content_input->openInputStreamFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_input, "invalid open input file '%s'"
            , Helper::getContentFullPath(  m_options.inputContent ).c_str()
        );

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( STRINGIZE_STRING_LOCAL( "pvrImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder for '%s'"
            , Helper::getContentFullPath( m_options.inputContent ).c_str()
        );

        if( decoder->prepareData( content_input, stream_input ) == false )
        {
            LOGGER_ERROR( "invalid prepare decoder for '%s'"
                , Helper::getContentFullPath( m_options.inputContent ).c_str()
            );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        uint32_t data_full_size = Helper::getImageCodecDataFullSize( dataInfo );

        MemoryInterfacePtr data_buffer = Helper::createMemoryCacheBuffer( data_full_size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data_buffer, "invalid cache memory [%u] for '%s'"
            , data_full_size
            , Helper::getContentFullPath( m_options.inputContent ).c_str()
        );

        void * data_memory = data_buffer->getBuffer();

        uint8_t * miplevel_data_memory = reinterpret_cast<uint8_t *>(data_memory);

        for( uint32_t i = 0; i != dataInfo->mipmaps; ++i )
        {
            if( decoder->rewind() == false )
            {
                LOGGER_ERROR( "invalid rewind '%s'"
                    , Helper::getContentFullPath( m_options.inputContent ).c_str()
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
                    , Helper::getContentFullPath( m_options.inputContent ).c_str()
                );

                return false;
            }

            miplevel_data_memory += miplevel_data_size;
        }

        decoder = nullptr;

        ContentInterfacePtr content_output = Helper::makeFileContent( m_fileGroupDev, full_inputFilePath, MENGINE_DOCUMENT_FACTORABLE );

        OutputStreamInterfacePtr stream_output = content_output->openOutputStreamFile( true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_output, "invalid open output '%s' for file '%s'"
            , Helper::getContentFullPath( m_options.outputContent ).c_str()
            , Helper::getContentFullPath( m_options.inputContent ).c_str()
        );

        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
            ->createEncoder( STRINGIZE_STRING_LOCAL( "htfImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( encoder, "invalid create encoder 'htfImage'" );

        if( encoder->initialize( content_output, stream_output ) == false )
        {
            LOGGER_ERROR( "invalid initialize encoder '%s'"
                , Helper::getContentFullPath( m_options.outputContent ).c_str()
            );

            return false;
        }

        ImageEncoderData htfEncoderData;
        htfEncoderData.buffer = data_memory;
        htfEncoderData.size = data_full_size;
        htfEncoderData.pitch = dataInfo->width * 1;

        ImageCodecDataInfo htfDataInfo;
        htfDataInfo.width = dataInfo->width;
        htfDataInfo.height = dataInfo->height;
        htfDataInfo.mipmaps = dataInfo->mipmaps;
        htfDataInfo.format = dataInfo->format;

        size_t encode_byte = encoder->encode( &htfEncoderData, &htfDataInfo );

        encoder->finalize();

        bool successful_close_stream = content_output->closeOutputStreamFile( stream_output );

        if( encode_byte == 0 )
        {
            LOGGER_ERROR( "'%s' invalid encode"
                , Helper::getContentFullPath( m_options.inputContent ).c_str()
            );

            return false;
        }

        if( successful_close_stream == false )
        {
            LOGGER_ERROR( "invalid close output '%s' for file '%s'"
                , Helper::getContentFullPath( content_output ).c_str()
                , Helper::getContentFullPath( m_options.inputContent ).c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}