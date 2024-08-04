#include "ParticleConverterPTCToPTZ.h"

#include "Plugins/AstralaxPlugin/AstralaxInterface.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Magic.h"
#include "Kernel/FilePath.h"
#include "Kernel/PathHelper.h"
#include "Kernel/StreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/VocabularyHelper.h"

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

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ParticleConverterPTCToPTZ::_finalize()
    {
        m_archivator = nullptr;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool ParticleConverterPTCToPTZ::convert()
    {
        const FileGroupInterfacePtr & inputFileGroup = m_options.inputContent->getFileGroup();
        const FilePath & inputFilePath = m_options.inputContent->getFilePath();

        const FileGroupInterfacePtr & outputFileGroup = m_options.outputContent->getFileGroup();
        const FilePath & outputFilePath = m_options.outputContent->getFilePath();

        const FilePath & inputFolderPath = inputFileGroup->getFolderPath();
        const FilePath & outputFolderPath = outputFileGroup->getFolderPath();

        FilePath full_inputFilePath = Helper::concatenateFilePath( {inputFolderPath, inputFilePath} );
        FilePath full_outputFilePath = Helper::concatenateFilePath( {outputFolderPath, outputFilePath} );

        MemoryInterfacePtr data_cache = Helper::createMemoryCacheFile( m_devFileGroup, full_inputFilePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data_cache );

        const void * data_memory = data_cache->getBuffer();
        size_t data_size = data_cache->getSize();

        MENGINE_ASSERTION_MEMORY_PANIC( data_memory, "invalid cache memory '%s'"
            , full_inputFilePath.c_str()
        );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_devFileGroup, full_outputFilePath, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open '%s'"
            , full_outputFilePath.c_str()
        );

        if( Helper::writeStreamArchiveMagic( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_PTZ ), GET_MAGIC_VERSION( MAGIC_PTZ ), false, data_memory, data_size, EAC_BEST ) == false )
        {
            LOGGER_ERROR( "invalid write '%s'"
                , full_outputFilePath.c_str()
            );

            return false;
        }

        if( Helper::closeOutputStreamFile( m_devFileGroup, stream ) == false )
        {
            return false;
        }

        stream = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ParticleConverterPTCToPTZ::validateVersion( const InputStreamInterfacePtr & _stream ) const
    {
        if( Helper::readStreamMagicHeader( _stream, GET_MAGIC_NUMBER( MAGIC_PTZ ), GET_MAGIC_VERSION( MAGIC_PTZ ) ) == false )
        {
            LOGGER_ERROR( "invalid magic header" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}