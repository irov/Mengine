#include "ParticleConverterPTCToPTZ.h"

#include "Plugins/AstralaxParticlePlugin/AstralaxInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Magic.h"
#include "Kernel/FilePath.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryHelper.h"

#include "Kernel/ResourceImage.h"

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
    bool ParticleConverterPTCToPTZ::_initialize()
    {
        m_convertExt = ".ptz";

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        m_archivator = archivator;

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ParticleConverterPTCToPTZ::convert()
    {
        const FilePath & pakPath = m_options.fileGroup->getFolderPath();

        FilePath full_input = Helper::concatenationFilePath( pakPath, m_options.inputFileName );
        FilePath full_output = Helper::concatenationFilePath( pakPath, m_options.outputFileName );

        MemoryInterfacePtr data_cache = Helper::createMemoryCacheFile( m_fileGroup, full_input, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( data_cache, false );

        const void * data_memory = data_cache->getBuffer();
        size_t data_size = data_cache->getSize();

        MENGINE_ASSERTION_MEMORY_PANIC( data_memory, false, "invalid cache memory '%s'"
            , full_input.c_str()
        );

        OutputStreamInterfacePtr output = FILE_SERVICE()
            ->openOutputFile( m_fileGroup, full_output, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( output, false, "invalid open '%s'"
            , full_output.c_str()
        );

        if( Helper::writeStreamArchiveData( output, m_archivator, GET_MAGIC_NUMBER( MAGIC_PTZ ), GET_MAGIC_VERSION( MAGIC_PTZ ), false, data_memory, data_size, EAC_BEST ) == false )
        {
            LOGGER_ERROR( "invalid write '%s'"
                , full_output.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ParticleConverterPTCToPTZ::validateVersion( const InputStreamInterfacePtr & _stream ) const
    {
        if( Helper::loadStreamMagicHeader( _stream, GET_MAGIC_NUMBER( MAGIC_PTZ ), GET_MAGIC_VERSION( MAGIC_PTZ ) ) == false )
        {
            LOGGER_ERROR( "invalid magic header"
            );

            return false;
        }

        return true;
    }
}