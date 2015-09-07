#	include "ParticleConverterPTCToPTZ.h"

#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/CacheInterface.h"

#	include "Logger/Logger.h"

#   include "Core/Magic.h"
#   include "Core/FilePath.h"
#	include "Core/MemoryCacheBuffer.h"
#	include "Core/Stream.h"

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
			->getArchivator( STRINGIZE_STRING_LOCAL(m_serviceProvider, "lz4") );

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

        ConstString full_input = Helper::concatenationFilePath( m_serviceProvider, pakPath, m_options.inputFileName );
        ConstString full_output = Helper::concatenationFilePath( m_serviceProvider, pakPath, m_options.outputFileName );
		        
		MemoryCacheBufferPtr data_cache = Helper::createMemoryFile( m_serviceProvider, STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev" ), full_input, false, "ParticleConverterPTCToPTZ_data" );

		if( data_cache == nullptr )
		{
			return false;
		}
			
		const Blobject::value_type * data_memory = data_cache->getMemoryT<Blobject::value_type *>();
		size_t data_size = data_cache->getSize();

		if( data_memory == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::convert_: invalid cache memory '%s'"
				, full_input.c_str()
				);

			return false;
		}

        OutputStreamInterfacePtr output = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev" ), full_output );

		if( output == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::convert_: invalid open '%s'"
				, full_output.c_str()
				);

			return false;
		}

		if( Helper::writeStreamArchiveData( m_serviceProvider, output, m_archivator, GET_MAGIC_NUMBER(MAGIC_PTZ), GET_MAGIC_VERSION(MAGIC_PTZ), false, data_memory, data_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::convert_: invalid write '%s'"
				, full_output.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleConverterPTCToPTZ::validateVersion( const InputStreamInterfacePtr & _stream ) const
	{
		if( Helper::loadStreamMagicHeader( m_serviceProvider, _stream, GET_MAGIC_NUMBER(MAGIC_PTZ), GET_MAGIC_VERSION(MAGIC_PTZ) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::validateVersion: invalid magic header"
				);

			return false;
		}
		
		return true;
	}
}