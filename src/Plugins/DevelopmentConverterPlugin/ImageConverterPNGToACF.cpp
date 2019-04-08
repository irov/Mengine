#include "ImageConverterPNGToACF.h"

#include "Interface/ImageCodecInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Kernel/Magic.h"
#include "Kernel/FilePath.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/MemoryHelper.h"

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
        m_convertExt = ".acf";

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ImageConverterPNGToACF::convert()
    {
        const FilePath & pakPath = m_options.fileGroup->getFolderPath();

        FilePath full_input = Helper::concatenationFilePath( pakPath, m_options.inputFileName );
        FilePath full_output = Helper::concatenationFilePath( pakPath, m_options.outputFileName );

        InputStreamInterfacePtr stream_intput = FILE_SERVICE()
            ->openInputFile( m_fileGroup, full_input, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream_intput == nullptr )
        {
            LOGGER_ERROR( "invalid open input file '%s'"
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "pngImage" ), MENGINE_DOCUMENT_FUNCTION );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "invalid create decoder '%s'"
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        if( decoder->prepareData( stream_intput ) == false )
        {
            LOGGER_ERROR( "invalid prepare decoder '%s'"
                , m_options.inputFileName.c_str()
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
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        size_t data_size = dataInfo->getSize();

        MemoryInterfacePtr data_buffer = Helper::createMemoryCacheBuffer( data_size, "ImageConverterPNGToACF", __FILE__, __LINE__ );

        if( data_buffer == nullptr )
        {
            return false;
        }

        void * data_memory = data_buffer->getBuffer();

        if( decoder->decode( data_memory, data_size ) == 0 )
        {
            LOGGER_ERROR( "invalid decode '%s'"
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        OutputStreamInterfacePtr stream_output = FILE_SERVICE()
            ->openOutputFile( m_fileGroup, full_output, MENGINE_DOCUMENT_FUNCTION );

        if( stream_output == nullptr )
        {
            LOGGER_ERROR( "%s invalid open output '%s'"
                , m_options.inputFileName.c_str()
                , full_output.c_str()
            );

            return false;
        }

        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
            ->createEncoderT<ImageEncoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "acfImage" ), MENGINE_DOCUMENT_FUNCTION );

        if( encoder == nullptr )
        {
            LOGGER_ERROR( "%s invalid create encoder"
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        if( encoder->initialize( stream_output ) == false )
        {
            LOGGER_ERROR( "%s invalid initialize encoder"
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        ImageCodecOptions encoder_options;
        encoder_options.channels = 1;
        encoder_options.pitch = dataInfo->width;

        if( encoder->setOptions( &encoder_options ) == false )
        {
            LOGGER_ERROR( "%s invalid optionize encoder"
                , m_options.inputFileName.c_str()
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
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        return true;
    }
}