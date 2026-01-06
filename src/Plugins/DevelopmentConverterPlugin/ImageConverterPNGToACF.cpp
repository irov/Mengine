#include "ImageConverterPNGToACF.h"

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
    ImageConverterPNGToACF::ImageConverterPNGToACF()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ImageConverterPNGToACF::~ImageConverterPNGToACF()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ImageConverterPNGToACF::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".acf" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImageConverterPNGToACF::_finalize()
    {
        //Empty
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ImageConverterPNGToACF::convert()
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
            , Helper::getContentFullPath( m_options.inputContent ).c_str()
        );

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( STRINGIZE_STRING_LOCAL( "pngImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder '%s'"
            , Helper::getContentFullPath( m_options.inputContent ).c_str()
        );

        if( decoder->prepareData( content_input, stream_input ) == false )
        {
            LOGGER_ERROR( "invalid prepare decoder '%s'"
                , Helper::getContentFullPath( m_options.inputContent ).c_str()
            );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        size_t data_size = Helper::getImageCodecDataSize( dataInfo );

        MemoryInterfacePtr data_buffer = Helper::createMemoryCacheBuffer( data_size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data_buffer, "invalid create cache memory '%s'"
            , Helper::getContentFullPath( m_options.inputContent ).c_str()
        );

        void * data_memory = data_buffer->getBuffer();

        ImageDecoderData htfDecoderData;
        htfDecoderData.buffer = data_memory;
        htfDecoderData.size = data_size;
        htfDecoderData.pitch = dataInfo->width;
        htfDecoderData.format = PF_L8;

        if( decoder->decode( &htfDecoderData ) == 0 )
        {
            LOGGER_ERROR( "invalid decode '%s'"
                , Helper::getContentFullPath( m_options.inputContent ).c_str()
            );

            return false;
        }

        decoder = nullptr;

        ContentInterfacePtr content_output = Helper::makeFileContent( m_fileGroupDev, full_outputFilePath, MENGINE_DOCUMENT_FACTORABLE );

        OutputStreamInterfacePtr stream_output = content_output->openOutputStreamFile( true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_output, "'%s' invalid open output '%s'"
            , Helper::getContentFullPath( m_options.inputContent ).c_str()
            , Helper::getContentFullPath( m_options.outputContent ).c_str()
        );

        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
            ->createEncoder( STRINGIZE_STRING_LOCAL( "acfImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( encoder, "'%s' invalid create encoder"
            , Helper::getContentFullPath( m_options.inputContent ).c_str()
        );

        if( encoder->initialize( content_output, stream_output ) == false )
        {
            LOGGER_ERROR( "%s invalid initialize encoder"
                , Helper::getContentFullPath( m_options.inputContent ).c_str()
            );

            return false;
        }

        ImageEncoderData htfEncoderData;
        htfEncoderData.buffer = data_memory;
        htfEncoderData.size = data_size;
        htfEncoderData.pitch = dataInfo->width;

        ImageCodecDataInfo htfDataInfo;
        htfDataInfo.width = dataInfo->width;
        htfDataInfo.height = dataInfo->height;
        htfDataInfo.mipmaps = 1;
        htfDataInfo.format = PF_UNKNOWN;

        size_t encode_byte = encoder->encode( &htfEncoderData, &htfDataInfo );

        encoder->finalize();

        bool successful_close_stream = content_output->closeOutputStreamFile( stream_output );

        if( encode_byte == 0 )
        {
            LOGGER_ERROR( "%s invalid encode"
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