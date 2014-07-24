#	include "ImageConverterPVRToHTF.h"

#	include "Interface/ParticleSystemInterface2.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/CacheInterface.h"

#	include "Logger/Logger.h"

#   include "Core/Magic.h"
#   include "Core/FilePath.h"
#	include "Core/CacheMemoryBuffer.h"

#   include "Config/Blobject.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
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
	bool ImageConverterPVRToHTF::initialize()
	{
        m_convertExt = ".ptz";

		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "zip") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ImageConverterPVRToHTF::convert()
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

        ConstString full_input = concatenationFilePath( m_serviceProvider, pakPath, m_options.inputFileName );
        ConstString full_output = concatenationFilePath( m_serviceProvider, pakPath, m_options.outputFileName );
		        
        InputStreamInterfacePtr stream_intput = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev" ), full_input, false );

		if( stream_intput == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid open input file"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		ImageDecoderInterfacePtr decoder = CODEC_SERVICE(m_serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( CONST_STRING_LOCAL(m_serviceProvider, "pvrImage") );

		if( decoder == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid create decoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		if( decoder->prepareData( stream_intput ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid prepare decoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		const ImageCodecDataInfo * dataInfo = decoder->getCodecDataInfo();

		ImageCodecOptions decoder_options;
		decoder_options.channels = 3;
		decoder_options.pitch = dataInfo->width;		

		if( decoder->setOptions( &decoder_options ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid optionize decoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}	
		
		size_t data_size = dataInfo->size;
		CacheMemoryBuffer data_buffer(m_serviceProvider, data_size, "ImageConverterPVRToHTF_data");
		void * data_memory = data_buffer.getMemory();

		if( decoder->decode( data_memory, data_size ) == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid decode"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

        OutputStreamInterfacePtr stream_output = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev" ), full_output );

		if( stream_output == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid open output %s"
				, m_options.inputFileName.c_str()
				, full_output.c_str()
				);

			return false;
		}

		ImageEncoderInterfacePtr encoder = CODEC_SERVICE(m_serviceProvider)
			->createEncoderT<ImageEncoderInterfacePtr>( CONST_STRING_LOCAL(m_serviceProvider, "htfImage") );

		if( encoder->initialize( stream_output ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid initialize encoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		ImageCodecOptions encoder_options;
		encoder_options.channels = 3;
		encoder_options.pitch = dataInfo->width;		

		if( encoder->setOptions( &encoder_options ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid optionize encoder"
				, m_options.inputFileName.c_str()
				);

			return false;
		}

		ImageCodecDataInfo htfDataInfo;
		htfDataInfo.size = dataInfo->size;
		htfDataInfo.width = dataInfo->width;
		htfDataInfo.height = dataInfo->height;
		htfDataInfo.depth = 1;
		htfDataInfo.mipmaps = 0;
		htfDataInfo.channels = 3;
		htfDataInfo.format = dataInfo->format;

		size_t encode_byte = encoder->encode( data_memory, &htfDataInfo );

		if( encode_byte == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ImageConverterPVRToHTF::convert: %s invalid encode"
				, m_options.inputFileName.c_str()
				);

			return false;
		}
				
        return true;
	}
}