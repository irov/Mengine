#	include "ModelConverterMDLToMDZ.h"

#	include "Interface/Model3DInterface.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/CacheInterface.h"

#	include "Logger/Logger.h"

#   include "Core/FilePath.h"
#	include "Core/CacheMemoryBuffer.h"
#	include "Core/Stream.h"

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
			->getArchivator( CONST_STRING_LOCAL(m_serviceProvider, "lz4") );

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
            LOGGER_ERROR(m_serviceProvider)("ModelConverterMDLToMDZ::convert_: not found file group '%s'"
                , m_options.pakName.c_str()
                );

            return false;
        }

        const ConstString & pakPath = fileGroup->getPath();            

        ConstString full_input = concatenationFilePath( m_serviceProvider, pakPath, m_options.inputFileName );
        ConstString full_output = concatenationFilePath( m_serviceProvider, pakPath, m_options.outputFileName );

        ConstString c_dev = CONST_STRING_LOCAL( m_serviceProvider, "dev" );

        InputStreamInterfacePtr input = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( c_dev, full_input, false );

        size_t uncompressSize = input->size();

		CacheMemoryBuffer data_buffer(m_serviceProvider, uncompressSize, "ModelConverterMDLToMDZ_data");
		TBlobject::value_type * data_memory = data_buffer.getMemoryT<TBlobject::value_type>();

        input->read( data_memory, uncompressSize );

		OutputStreamInterfacePtr output = FILE_SERVICE(m_serviceProvider)
			->openOutputFile( CONST_STRING_LOCAL( m_serviceProvider, "dev" ), full_output );

		if( output == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ModelConverterMDLToMDZ::convert_: invalid open '%s'"
				, full_output.c_str()
				);

			return false;
		}

		if( Helper::writeStreamArchiveData( m_serviceProvider, output, m_archivator, GET_MAGIC_NUMBER(MAGIC_MDL), GET_MAGIC_VERSION(MAGIC_MDL), false, data_memory, uncompressSize ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ModelConverterMDLToMDZ::convert_: invalid write '%s'"
				, full_output.c_str()
				);

			return false;
		}

        return true;
	}
}