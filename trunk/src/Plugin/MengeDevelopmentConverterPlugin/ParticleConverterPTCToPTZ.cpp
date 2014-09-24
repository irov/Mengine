#	include "ParticleConverterPTCToPTZ.h"

#	include "Interface/ParticleSystemInterface2.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/CacheInterface.h"

#	include "Logger/Logger.h"

#   include "Core/Magic.h"
#   include "Core/FilePath.h"
#	include "Core/CacheMemoryBuffer.h"
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
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "lz4") );

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
            ->openInputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev" ), full_input, false );

        size_t uncompressSize = input->size();

		CacheMemoryBuffer data_buffer(m_serviceProvider, uncompressSize, "ParticleConverterPTCToPTZ_data");
		TBlobject::value_type * data_memory = data_buffer.getMemoryT<TBlobject::value_type>();

        input->read( data_memory, uncompressSize );
		
        OutputStreamInterfacePtr output = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev" ), full_output );

		if( output == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::convert_: invalid open '%s'"
				, full_output.c_str()
				);

			return false;
		}

		if( Helper::writeStreamArchiveData( m_serviceProvider, output, m_archivator, GET_MAGIC_NUMBER(MAGIC_PTZ), GET_MAGIC_VERSION(MAGIC_PTZ), false, data_memory, uncompressSize ) == false )
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
		unsigned char * binary_memory;
		size_t binary_size;

		if( Helper::loadStreamMagicHeader( m_serviceProvider, _stream, GET_MAGIC_NUMBER(MAGIC_PTZ), GET_MAGIC_VERSION(MAGIC_PTZ) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ParticleConverterPTCToPTZ::validateVersion: invalid magic header"
				);

			return false;
		}
		
		return true;
	}
}