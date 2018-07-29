#include "ModelConverterMDLToMDZ.h"

#include "Interface/Model3DInterface.h"

#include "Interface/StringizeInterface.h"
#include "Interface/ArchiveInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Kernel/Logger.h"

#include "Kernel/FilePath.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryHelper.h"

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
	bool ModelConverterMDLToMDZ::_initialize()
	{
        m_convertExt = ".mdz";

        const ArchivatorInterfacePtr & archivator = ARCHIVE_SERVICE()
			->getArchivator( STRINGIZE_STRING_LOCAL( "lz4") );

		if( archivator == nullptr )
		{
			return false;
		}

        m_archivator = archivator;

		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool ModelConverterMDLToMDZ::convert()
	{
        const FilePath & pakPath = m_options.fileGroup->getFolderPath();

		FilePath full_input = Helper::concatenationFilePath( pakPath, m_options.inputFileName );
		FilePath full_output = Helper::concatenationFilePath( pakPath, m_options.outputFileName );

		MemoryInterfacePtr cache = Helper::createMemoryCacheFile( m_fileGroup, full_input, false, "ModelConverterMDLToMDZ", __FILE__, __LINE__ );

		if( cache == nullptr )
		{
			LOGGER_ERROR("ModelConverterMDLToMDZ::convert_: invalid create memory file '%s'"
				, full_input.c_str()
				);

			return false;
		}
				
		void * data_memory = cache->getBuffer();
		size_t uncompressSize = cache->getSize();

		OutputStreamInterfacePtr output = FILE_SERVICE()
			->openOutputFile( m_fileGroup, full_output );

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