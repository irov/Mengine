#include "ModelConverterMDLToMDZ.h"

#include "Interface/Model3DInterface.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ArchiveInterface.h"
#include "Interface/MemoryInterface.h"

#include "Logger/Logger.h"

#include "Core/FilePath.h"
#include "Core/Stream.h"
#include "Core/MemoryHelper.h"

#include "Config/Blobject.h"

namespace Mengine
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

		m_archivator = ARCHIVE_SERVICE()
			->getArchivator( STRINGIZE_STRING_LOCAL( "lz4") );

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
        if( FILE_SERVICE()->hasFileGroup( m_options.fileGroup, &fileGroup ) == false )
        {
            LOGGER_ERROR("ModelConverterMDLToMDZ::convert_: not found file group '%s'"
                , m_options.fileGroup.c_str()
                );

            return false;
        }

        const FilePath & pakPath = fileGroup->getFolderPath();

		FilePath full_input = Helper::concatenationFilePath( pakPath, m_options.inputFileName );
		FilePath full_output = Helper::concatenationFilePath( pakPath, m_options.outputFileName );

        ConstString c_dev = STRINGIZE_STRING_LOCAL( "dev" );

		MemoryInterfacePtr cache = Helper::createMemoryCacheFile( c_dev, full_input, false, __FILE__, __LINE__ );

		if( cache == nullptr )
		{
			LOGGER_ERROR("ModelConverterMDLToMDZ::convert_: invalid create memory file '%s'"
				, full_input.c_str()
				);

			return false;
		}
				
		void * data_memory = cache->getMemory();
		size_t uncompressSize = cache->getSize();

		OutputStreamInterfacePtr output = FILE_SERVICE()
			->openOutputFile( STRINGIZE_STRING_LOCAL( "dev" ), full_output );

		if( output == nullptr )
		{
			LOGGER_ERROR("ModelConverterMDLToMDZ::convert_: invalid open '%s'"
				, full_output.c_str()
				);

			return false;
		}

		if( Helper::writeStreamArchiveData( output, m_archivator, GET_MAGIC_NUMBER( MAGIC_MDL ), GET_MAGIC_VERSION( MAGIC_MDL ), false, data_memory, uncompressSize, EAC_BEST ) == false )
		{
			LOGGER_ERROR("ModelConverterMDLToMDZ::convert_: invalid write '%s'"
				, full_output.c_str()
				);

			return false;
		}

        return true;
	}
}