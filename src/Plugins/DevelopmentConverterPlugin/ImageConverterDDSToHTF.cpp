#include "ImageConverterDDSToHTF.h"

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
        m_convertExt = ".htf";

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

        if( fileGroup == nullptr )
        {
            return false;
        }

        m_fileGroup = fileGroup;

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ImageConverterDDSToHTF::convert()
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
            ->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "ddsImage" ), MENGINE_DOCUMENT_FUNCTION );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "invalid create decoder for '%s'"
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        if( decoder->prepareData( stream_intput ) == false )
        {
            LOGGER_ERROR( "invalid prepare decoder for '%s'"
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        uint32_t data_full_size = dataInfo->getFullSize();

        MemoryInterfacePtr data_buffer = Helper::createMemoryCacheBuffer( data_full_size, "ImageConverterDDSToHTF", __FILE__, __LINE__ );

        if( data_buffer == nullptr )
        {
            LOGGER_ERROR( "invalid cache memory %d for '%s'"
                , data_full_size
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        void * data_memory = data_buffer->getBuffer();

        uint8_t * miplevel_data_memory = reinterpret_cast<uint8_t *>(data_memory);

        for( uint32_t i = 0; i != dataInfo->mipmaps; ++i )
        {
            ImageCodecOptions decoder_options;

            uint32_t miplevel_data_size = dataInfo->getMipMapSize( i );

            decoder_options.pitch = miplevel_data_size / (dataInfo->height >> i);
            decoder_options.channels = 3;

            if( decoder->setOptions( &decoder_options ) == false )
            {
                LOGGER_ERROR( "invalid optionize decoder '%s'"
                    , m_options.inputFileName.c_str()
                );

                return false;
            }

            if( decoder->rewind() == false )
            {
                LOGGER_ERROR( "invalid rewind '%s'"
                    , m_options.inputFileName.c_str()
                );

                return false;
            }

            if( decoder->decode( miplevel_data_memory, miplevel_data_size ) == 0 )
            {
                LOGGER_ERROR( "invalid decode '%s'"
                    , m_options.inputFileName.c_str()
                );

                return false;
            }

            miplevel_data_memory += miplevel_data_size;
        }

        OutputStreamInterfacePtr stream_output = FILE_SERVICE()
            ->openOutputFile( m_fileGroup, full_output, MENGINE_DOCUMENT_FUNCTION );

        if( stream_output == nullptr )
        {
            LOGGER_ERROR( "invalid open output '%s' for '%s'"
                , full_output.c_str()
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
            ->createEncoderT<ImageEncoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "htfImage" ), MENGINE_DOCUMENT_FUNCTION );

        if( encoder->initialize( stream_output ) == false )
        {
            LOGGER_ERROR( "ImageConverterDDSToHTF::convert: %s invalid initialize encoder"
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        ImageCodecOptions encoder_options;
        encoder_options.channels = 3;
        encoder_options.pitch = dataInfo->width;

        if( encoder->setOptions( &encoder_options ) == false )
        {
            LOGGER_ERROR( "ImageConverterDDSToHTF::convert: %s invalid optionize encoder"
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        ImageCodecDataInfo htfDataInfo;
        htfDataInfo.width = dataInfo->width;
        htfDataInfo.height = dataInfo->height;
        htfDataInfo.depth = 1;
        htfDataInfo.mipmaps = dataInfo->mipmaps;
        htfDataInfo.channels = 3;
        htfDataInfo.format = dataInfo->format;

        size_t encode_byte = encoder->encode( data_memory, data_full_size, &htfDataInfo );

        if( encode_byte == 0 )
        {
            LOGGER_ERROR( "ImageConverterDDSToHTF::convert: %s invalid encode"
                , m_options.inputFileName.c_str()
            );

            return false;
        }

        return true;
    }
}