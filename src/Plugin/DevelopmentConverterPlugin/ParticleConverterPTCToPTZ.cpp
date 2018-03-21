#include "ParticleConverterPTCToPTZ.h"

#include "Interface/ParticleSystemInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ArchiveInterface.h"
#include "Interface/MemoryInterface.h"

#include "Logger/Logger.h"

#include "Core/Magic.h"
#include "Core/FilePath.h"
#include "Core/Stream.h"
#include "Core/MemoryHelper.h"

#include "Kernel/ResourceImage.h"

#include "Config/Blobject.h"

namespace Mengine
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

		m_archivator = ARCHIVE_SERVICE()
			->getArchivator( STRINGIZE_STRING_LOCAL( "lz4") );

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
        if( FILE_SERVICE()->hasFileGroup( m_options.pakName, &fileGroup ) == false )
        {
            LOGGER_ERROR("ParticleConverterPTCToPTZ::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const FilePath & pakPath = fileGroup->getFolderPath();

		FilePath full_input = Helper::concatenationFilePath( pakPath, m_options.inputFileName );
		FilePath full_output = Helper::concatenationFilePath( pakPath, m_options.outputFileName );
		        
		MemoryInterfacePtr data_cache = Helper::createMemoryCacheFile( STRINGIZE_STRING_LOCAL( "dev" ), full_input, false, __FILE__, __LINE__ );

		if( data_cache == nullptr )
		{
			return false;
		}
			
		const Blobject::value_type * data_memory = data_cache->getMemory();
		size_t data_size = data_cache->getSize();

		if( data_memory == nullptr )
		{
			LOGGER_ERROR("ParticleConverterPTCToPTZ::convert_: invalid cache memory '%s'"
				, full_input.c_str()
				);

			return false;
		}

        OutputStreamInterfacePtr output = FILE_SERVICE()
            ->openOutputFile( STRINGIZE_STRING_LOCAL( "dev" ), full_output );

		if( output == nullptr )
		{
			LOGGER_ERROR("ParticleConverterPTCToPTZ::convert_: invalid open '%s'"
				, full_output.c_str()
				);

			return false;
		}

		if( Helper::writeStreamArchiveData( output, m_archivator, GET_MAGIC_NUMBER( MAGIC_PTZ ), GET_MAGIC_VERSION( MAGIC_PTZ ), false, data_memory, data_size, EAC_BEST ) == false )
		{
			LOGGER_ERROR("ParticleConverterPTCToPTZ::convert_: invalid write '%s'"
				, full_output.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ParticleConverterPTCToPTZ::validateVersion( const InputStreamInterfacePtr & _stream ) const
	{
		if( Helper::loadStreamMagicHeader( _stream, GET_MAGIC_NUMBER(MAGIC_PTZ), GET_MAGIC_VERSION(MAGIC_PTZ) ) == false )
		{
			LOGGER_ERROR("ParticleConverterPTCToPTZ::validateVersion: invalid magic header"
				);

			return false;
		}
		
		return true;
	}
}