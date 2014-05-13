#	include "ParticleConverterPTCToPTZ.h"

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
	ParticleConverterPTCToPTZ::ParticleConverterPTCToPTZ()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	ParticleConverterPTCToPTZ::~ParticleConverterPTCToPTZ()
	{
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ParticleConverterPTCToPTZ::initialize()
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
	bool ParticleConverterPTCToPTZ::convert()
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
		        
        InputStreamInterfacePtr input = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev" ), full_input );

        uint32_t data_size = input->size();

		CacheMemoryBuffer data_buffer(m_serviceProvider, data_size, "ParticleConverterPTCToPTZ_data");
		TBlobject::value_type * data_memory = data_buffer.getMemoryT<TBlobject::value_type>();

        input->read( data_memory, data_size );

		MemoryInputPtr compress_memory = ARCHIVE_SERVICE(m_serviceProvider)
			->compress( m_archivator, data_memory, data_size );

		if( compress_memory == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::convert_: %s invalid compress"
				, m_options.inputFileName.c_str()
				);

			return false;
		}
		
        OutputStreamInterfacePtr output = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev" ), full_output );

			
        output->write( &data_size, sizeof(data_size) );

		uint32_t compress_size;
		const void * compress_buffer = compress_memory->getMemory( compress_size );

        output->write( &compress_size, sizeof(compress_size) );
        output->write( compress_buffer, compress_size );

        return true;
	}
}