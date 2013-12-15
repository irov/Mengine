#	include "ModelConverterMDLToMDZ.h"

#	include "Interface/Model3DInterface.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/CacheInterface.h"

#	include "Logger/Logger.h"
#   include "Core/FilePath.h"
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

        return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ModelConverterMDLToMDZ::convert()
	{
        FileGroupInterface * fileGroup;
        if( FILE_SERVICE(m_serviceProvider)->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleConverterPTCToPTZ::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const ConstString & pakPath = fileGroup->getPath();            

        ConstString full_input = concatenationFilePath( m_serviceProvider, pakPath, m_options.inputFileName );
        ConstString full_output = concatenationFilePath( m_serviceProvider, pakPath, m_options.outputFileName );

        ConstString c_dev = Helper::stringizeString( m_serviceProvider, "dev" );

        InputStreamInterfacePtr input = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( c_dev, full_input );

        size_t data_size = input->size();

		CacheBufferInterfacePtr data_buffer = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( data_size );
        
		TBlobject::value_type * data_memory = data_buffer->getMemory();

        input->read( data_memory, data_size );

        size_t archive_size = ARCHIVE_SERVICE(m_serviceProvider)
            ->compressBound( data_size );

        if( archive_size == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleConverterPTCToPTZ::convert_: %s invalid compressBound %d"
                , m_options.inputFileName.c_str()
                , data_size
                );

            return false;
        }

		CacheBufferInterfacePtr archive_buffer = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( archive_size );

		TBlobject::value_type * archive_memory = archive_buffer->getMemory();
		
        size_t comress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->compress( archive_memory, archive_size, comress_size, data_memory, data_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ParticleConverterPTCToPTZ::convert_: %s invalid compress"
                , m_options.inputFileName.c_str()
                );
			
            return false;
        }

        OutputStreamInterfacePtr output = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( c_dev, full_output );

		output->write( &DATAFLOW_MAGIC_MDL, sizeof(DATAFLOW_MAGIC_MDL) );
		output->write( &DATAFLOW_VERSION_MDL, sizeof(DATAFLOW_VERSION_MDL) );
        output->write( &data_size, sizeof(data_size) );
        output->write( &comress_size, sizeof(comress_size) );
        output->write( archive_memory, comress_size );

        return true;
	}
}