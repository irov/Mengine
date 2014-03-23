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

        ConstString c_dev = Helper::stringizeString( m_serviceProvider, "dev" );

        InputStreamInterfacePtr input = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( c_dev, full_input );

        size_t data_size = input->size();

		CacheMemoryBuffer data_buffer(m_serviceProvider, data_size);
		TBlobject::value_type * data_memory = data_buffer.getMemoryT<TBlobject::value_type>();

        input->read( data_memory, data_size );

        size_t archive_size = ARCHIVE_SERVICE(m_serviceProvider)
            ->compressBound( data_size );

        if( archive_size == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::convert_: %s invalid compressBound %d"
                , m_options.inputFileName.c_str()
                , data_size
                );

            return false;
        }

		CacheMemoryBuffer archive_buffer(m_serviceProvider, archive_size);
		TBlobject::value_type * archive_memory = archive_buffer.getMemoryT<TBlobject::value_type>();

        size_t comress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->compress( archive_memory, archive_size, data_memory, data_size, comress_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::convert_: %s invalid compress"
                , m_options.inputFileName.c_str()
                );

            return false;
        }

        OutputStreamInterfacePtr output = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( c_dev, full_output );

			
        output->write( &data_size, sizeof(data_size) );
        output->write( &comress_size, sizeof(comress_size) );
        output->write( archive_memory, comress_size );

        return true;
	}
}