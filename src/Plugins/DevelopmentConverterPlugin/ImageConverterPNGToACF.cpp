#include "ImageConverterPNGToACF.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Magic.h"
#include "Kernel/FilePath.h"
#include "Kernel/PathHelper.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/ConstStringHelper.h"

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
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ImageConverterPNGToACF::convert()
    {
        const FilePath & folderPath = m_options.fileGroup->getFolderPath();

        FilePath full_inputFilePath = Helper::concatenationFilePath( folderPath, m_options.inputFilePath );
        FilePath full_outputFilePath = Helper::concatenationFilePath( folderPath, m_options.outputFilePath );

        InputStreamInterfacePtr stream_intput = FILE_SERVICE()
            ->openInputFile( m_fileGroup, full_inputFilePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_intput, false, "invalid open input file '%s'"
            , m_options.inputFilePath.c_str()
        );

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "pngImage" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, false, "invalid create decoder '%s'"
            , m_options.inputFilePath.c_str()
        );

        if( decoder->prepareData( stream_intput ) == false )
        {
            LOGGER_ERROR( "invalid prepare decoder '%s'"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        ImageCodecOptions decoder_options;
        decoder_options.channels = 1;
        decoder_options.pitch = dataInfo->width;

        if( decoder->setOptions( &decoder_options ) == false )
        {
            LOGGER_ERROR( "invalid optionize decoder '%s'"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        size_t data_size = dataInfo->getSize();

        MemoryInterfacePtr data_buffer = Helper::createMemoryCacheBuffer( data_size, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( data_buffer, false );

        void * data_memory = data_buffer->getBuffer();

        if( decoder->decode( data_memory, data_size ) == 0 )
        {
            LOGGER_ERROR( "invalid decode '%s'"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        OutputStreamInterfacePtr stream_output = FILE_SERVICE()
            ->openOutputFile( m_fileGroup, full_outputFilePath, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_output, false, "'%s' invalid open output '%s'"
            , m_options.inputFilePath.c_str()
            , full_outputFilePath.c_str()
        );

        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
            ->createEncoderT<ImageEncoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "acfImage" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( encoder, false, "'%s' invalid create encoder"
            , m_options.inputFilePath.c_str()
        );

        if( encoder->initialize( stream_output ) == false )
        {
            LOGGER_ERROR( "%s invalid initialize encoder"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        ImageCodecOptions encoder_options;
        encoder_options.channels = 1;
        encoder_options.pitch = dataInfo->width;

        if( encoder->setOptions( &encoder_options ) == false )
        {
            LOGGER_ERROR( "%s invalid optionize encoder"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        ImageCodecDataInfo htfDataInfo;
        htfDataInfo.width = dataInfo->width;
        htfDataInfo.height = dataInfo->height;
        htfDataInfo.depth = 1;
        htfDataInfo.mipmaps = 1;
        htfDataInfo.channels = 1;
        htfDataInfo.format = PF_UNKNOWN;

        size_t encode_byte = encoder->encode( data_memory, data_size, &htfDataInfo );

        if( encode_byte == 0 )
        {
            LOGGER_ERROR( "%s invalid encode"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        return true;
    }
}