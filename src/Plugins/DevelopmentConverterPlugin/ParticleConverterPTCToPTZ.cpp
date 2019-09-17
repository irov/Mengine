#include "ParticleConverterPTCToPTZ.h"

#include "Plugins/AstralaxParticlePlugin/AstralaxInterface.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Magic.h"
#include "Kernel/FilePath.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
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
        m_convertExt = STRINGIZE_STRING_LOCAL( ".ptz" );

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, false );

        m_archivator = archivator;

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ParticleConverterPTCToPTZ::convert()
    {
        const FilePath & folderPath = m_options.fileGroup->getFolderPath();

        FilePath full_inputFilePath = Helper::concatenationFilePath( folderPath, m_options.inputFilePath );
        FilePath full_outputFilePath = Helper::concatenationFilePath( folderPath, m_options.outputFilePath );

        MemoryInterfacePtr data_cache = Helper::createMemoryCacheFile( m_fileGroup, full_inputFilePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( data_cache, false );

        const void * data_memory = data_cache->getBuffer();
        size_t data_size = data_cache->getSize();

        MENGINE_ASSERTION_MEMORY_PANIC( data_memory, false, "invalid cache memory '%s'"
            , full_inputFilePath.c_str()
        );

        OutputStreamInterfacePtr output = Helper::openOutputStreamFile( m_fileGroup, full_outputFilePath, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( output, false, "invalid open '%s'"
            , full_outputFilePath.c_str()
        );

        if( Helper::writeStreamArchiveData( output, m_archivator, GET_MAGIC_NUMBER( MAGIC_PTZ ), GET_MAGIC_VERSION( MAGIC_PTZ ), false, data_memory, data_size, EAC_BEST ) == false )
        {
            LOGGER_ERROR( "invalid write '%s'"
                , full_outputFilePath.c_str()
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