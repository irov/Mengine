#include "ImageConverterPVRToHTF.h"

#include "Interface/ParticleSystemInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ArchiveInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Kernel/Logger.h"

#include "Kernel/Magic.h"
#include "Kernel/FilePath.h"
#include "Kernel/MemoryHelper.h"

#include "Config/Blobject.h"

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
        m_convertExt = ".htf";

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ImageConverterPVRToHTF::convert()
	{
        const FilePath & pakPath = m_options.fileGroup->getFolderPath();

		FilePath full_input = Helper::concatenationFilePath( pakPath, m_options.inputFileName );
		FilePath full_output = Helper::concatenationFilePath( pakPath, m_options.outputFileName );
		        
        InputStreamInterfacePtr stream_intput = FILE_SERVICE()
            ->openInputFile( m_fileGroup, full_input, false );

		if( stream_intput == nullptr )
		{
			LOGGER_ERROR("ImageConverterPVRToHTF::convert: %s invalid open input file"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		ImageDecoderInterfacePtr decoder = CODEC_SERVICE()
			->createDecoderT<ImageDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "pvrImage") );

		if( decoder == nullptr )
		{
			LOGGER_ERROR("ImageConverterPVRToHTF::convert: %s invalid create decoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		if( decoder->prepareData( stream_intput ) == false )
		{
			LOGGER_ERROR("ImageConverterPVRToHTF::convert: %s invalid prepare decoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

        uint32_t data_full_size = dataInfo->getFullSize();

		MemoryInterfacePtr data_buffer = Helper::createMemoryCacheBuffer( data_full_size, __FILE__, __LINE__ );
		
		if( data_buffer == nullptr )
		{
			LOGGER_ERROR("ImageConverterPVRToHTF::convert: %s invalid cache memory %d"
				, m_options.inputFileName.c_str()
				, data_full_size
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
				LOGGER_ERROR("ImageConverterPVRToHTF::convert: %s invalid optionize decoder"
					, m_options.inputFileName.c_str()
					);

				return false;
			}	

			if( decoder->rewind() == false )
			{
				LOGGER_ERROR("ImageConverterPVRToHTF::convert: %s invalid rewind"
					, m_options.inputFileName.c_str()
					);

				return false;
			}

			if( decoder->decode( miplevel_data_memory, miplevel_data_size ) == 0 )
			{
				LOGGER_ERROR("ImageConverterDDSToHTF::convert: %s invalid decode"
					, m_options.inputFileName.c_str()
					);

				return false;
			}

			miplevel_data_memory += miplevel_data_size;
		}

        OutputStreamInterfacePtr stream_output = FILE_SERVICE()
            ->openOutputFile( m_fileGroup, full_output );

		if( stream_output == nullptr )
		{
			LOGGER_ERROR("ImageConverterPVRToHTF::convert: %s invalid open output %s"
				, m_options.inputFileName.c_str()
				, full_output.c_str()
				);

			return false;
		}

		ImageEncoderInterfacePtr encoder = CODEC_SERVICE()
			->createEncoderT<ImageEncoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "htfImage") );

		if( encoder->initialize( stream_output ) == false )
		{
			LOGGER_ERROR("ImageConverterPVRToHTF::convert: %s invalid initialize encoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		ImageCodecOptions encoder_options;
		encoder_options.channels = 3;
		encoder_options.pitch = dataInfo->width;		

		if( encoder->setOptions( &encoder_options ) == false )
		{
			LOGGER_ERROR("ImageConverterPVRToHTF::convert: %s invalid optionize encoder"
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
			LOGGER_ERROR("ImageConverterPVRToHTF::convert: %s invalid encode"
				, m_options.inputFileName.c_str()
				);

			return false;
		}
				
        return true;
	}
}