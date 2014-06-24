#	include "ModelConverterMDLToMDZ.h"

#	include "Interface/Model3DInterface.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/CacheInterface.h"

#	include "Logger/Logger.h"

#   include "Core/FilePath.h"
#	include "Core/CacheMemoryBuffer.h"

#   include "Config/Blobject.h"

#   include "WindowsLayer/WindowsIncluder.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	ModelConverterMDLToMDZ::ModelConverterMDLToMDZ()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	ModelConverterMDLToMDZ::~ModelConverterMDLToMDZ()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ModelConverterMDLToMDZ::initialize()
	{
        m_convertExt = ".mdz";

		m_archivator = ARCHIVE_SERVICE(m_serviceProvider)
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "zip") );

		if( m_archivator == nullptr )
		{
			return false;
		}

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ModelConverterMDLToMDZ::convert()
	{
        FileGroupInterfacePtr fileGroup;
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const ConstString & pakPath = fileGroup->getPath();            

        ConstString full_input = concatenationFilePath( m_serviceProvider, pakPath, m_options.inputFileName );
        ConstString full_output = concatenationFilePath( m_serviceProvider, pakPath, m_options.outputFileName );

        ConstString c_dev = CONST_STRING_LOCAL( m_serviceProvider, "dev" );

        InputStreamInterfacePtr input = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( c_dev, full_input );

        uint32_t data_size = input->size();

		CacheMemoryBuffer data_buffer(m_serviceProvider, data_size, "ModelConverterMDLToMDZ_data");
		TBlobject::value_type * data_memory = data_buffer.getMemoryT<TBlobject::value_type>();

        input->read( data_memory, data_size );

		MemoryInputPtr compress_memory = ARCHIVE_SERVICE(m_serviceProvider)
			->compressBuffer( m_archivator, data_memory, data_size );

        if( compress_memory == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::convert_: %s invalid compress"
                , m_options.inputFileName.c_str()
                );

            return false;
        }

		OutputStreamInterfacePtr output = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( c_dev, full_output );

		magic_number_type magic_number = GET_MAGIC_NUMBER(MAGIC_MDL);
		output->write( &magic_number, sizeof(magic_number) );

		magic_version_type magic_version = GET_MAGIC_NUMBER(MAGIC_MDL);
		output->write( &magic_version, sizeof(magic_version) );

        output->write( &data_size, sizeof(data_size) );
		
		uint32_t compress_size;
		const void * compress_buffer = compress_memory->getMemory( compress_size );

		output->write( &compress_size, sizeof(compress_size) );
		output->write( compress_buffer, compress_size );

        return true;
	}
}