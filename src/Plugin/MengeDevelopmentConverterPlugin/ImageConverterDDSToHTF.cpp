#	include "ImageConverterDDSToHTF.h"

#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/MemoryInterface.h"

#	include "Logger/Logger.h"

#   include "Core/Magic.h"
#   include "Core/FilePath.h"
#	include "Core/MemoryHelper.h"

#   include "Config/Blobject.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
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
	bool ImageConverterDDSToHTF::initialize()
	{
        m_convertExt = ".htf";

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ImageConverterDDSToHTF::convert()
	{
        FileGroupInterfacePtr fileGroup;
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const ConstString & pakPath = fileGroup->getPath();            

        ConstString full_input = Helper::concatenationFilePath( m_serviceProvider, pakPath, m_options.inputFileName );
        ConstString full_output = Helper::concatenationFilePath( m_serviceProvider, pakPath, m_options.outputFileName );
		        
        InputStreamInterfacePtr stream_intput = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev" ), full_input, false );

		if( stream_intput == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid open input file"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		ImageDecoderInterfacePtr decoder = CODEC_SERVICE(m_serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "ddsImage") );

		if( decoder == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid create decoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		if( decoder->prepareData( stream_intput ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterDDSToHTF::convert: %s invalid prepare decoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

		size_t data_full_size = dataInfo->getFullSize();

		MemoryCacheBufferInterfacePtr data_buffer = Helper::createMemoryBuffer( m_serviceProvider, data_full_size, "ImageConverterDDSToHTF_data" );

		if( data_buffer == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("ImageConverterDDSToHTF::convert: %s invalid cache memory %d"
				, m_options.inputFileName.c_str()
				, data_full_size
				);

			return false;
		}

		unsigned char * miplevel_data_memory = data_buffer->getMemoryT<unsigned char *>();

		for( uint32_t i = 0; i != dataInfo->mipmaps; ++i )
		{
			ImageCodecOptions decoder_options;

			uint32_t miplevel_data_size = dataInfo->getMipMapSize( i );

			decoder_options.pitch = miplevel_data_size / (dataInfo->height >> i);
			decoder_options.channels = 3;

			if( decoder->setOptions( &decoder_options ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid optionize decoder"
					, m_options.inputFileName.c_str()
					);

				return false;
			}

			if( decoder->rewind() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ImageConverterDDSToHTF::convert: %s invalid rewind"
					, m_options.inputFileName.c_str()
					);

				return false;
			}
			
			if( decoder->decode( miplevel_data_memory, miplevel_data_size ) == 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("ImageConverterDDSToHTF::convert: %s invalid decode"
					, m_options.inputFileName.c_str()
					);

				return false;
			}

			miplevel_data_memory += miplevel_data_size;
		}

        OutputStreamInterfacePtr stream_output = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev" ), full_output );

		if( stream_output == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterDDSToHTF::convert: %s invalid open output %s"
				, m_options.inputFileName.c_str()
				, full_output.c_str()
				);

			return false;
		}

		ImageEncoderInterfacePtr encoder = CODEC_SERVICE(m_serviceProvider)
			->createEncoderT<ImageEncoderInterfacePtr>( STRINGIZE_STRING_LOCAL(m_serviceProvider, "htfImage") );

		if( encoder->initialize( stream_output ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterDDSToHTF::convert: %s invalid initialize encoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		ImageCodecOptions encoder_options;
		encoder_options.channels = 3;
		encoder_options.pitch = dataInfo->width;		

		if( encoder->setOptions( &encoder_options ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterDDSToHTF::convert: %s invalid optionize encoder"
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

		size_t encode_byte = encoder->encode( miplevel_data_memory, &htfDataInfo );

		if( encode_byte == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterDDSToHTF::convert: %s invalid encode"
				, m_options.inputFileName.c_str()
				);

			return false;
		}
				
        return true;
	}
}