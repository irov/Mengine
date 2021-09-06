#include "ImageConverterPNGToACF.h"

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
        const FilePath & folderPath = m_options.fileGroup->getFolderPath();

        FilePath full_inputFilePath = Helper::concatenationFilePath( folderPath, m_options.inputFilePath );
        FilePath full_outputFilePath = Helper::concatenationFilePath( folderPath, m_options.outputFilePath );

        InputStreamInterfacePtr stream_intput = Helper::openInputStreamFile( m_fileGroup, full_inputFilePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_intput, "invalid open input file '%s'"
            , m_options.inputFilePath.c_str()
        );

        ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( STRINGIZE_STRING_LOCAL( "pngImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder '%s'"
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

        size_t data_size = Helper::getImageCodecDataSize( dataInfo );

        MemoryInterfacePtr data_buffer = Helper::createMemoryCacheBuffer( data_size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data_buffer );

        void * data_memory = data_buffer->getBuffer();

        ImageDecoderData htfDecoderData;
        htfDecoderData.buffer = data_memory;
        htfDecoderData.size = data_size;
        htfDecoderData.pitch = dataInfo->width;
        htfDecoderData.format = PF_L8;

        if( decoder->decode( &htfDecoderData ) == 0 )
        {
            LOGGER_ERROR( "invalid decode '%s'"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        OutputStreamInterfacePtr stream_output = Helper::openOutputStreamFile( m_fileGroup, full_outputFilePath, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream_output, "'%s' invalid open output '%s'"
            , m_options.inputFilePath.c_str()
            , full_outputFilePath.c_str()
        );

        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
            ->createEncoder( STRINGIZE_STRING_LOCAL( "acfImage" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( encoder, "'%s' invalid create encoder"
            , m_options.inputFilePath.c_str()
        );

        if( encoder->initialize( stream_output ) == false )
        {
            LOGGER_ERROR( "%s invalid initialize encoder"
                , m_options.inputFilePath.c_str()
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

        if( encode_byte == 0 )
        {
            LOGGER_ERROR( "%s invalid encode"
                , m_options.inputFilePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}