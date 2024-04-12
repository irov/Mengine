#include "DazzleEffectConverterDZBToDZZ.h"

#include "DazzleInterface.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Magic.h"
#include "Kernel/FilePath.h"
#include "Kernel/PathHelper.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    DazzleEffectConverterDZBToDZZ::DazzleEffectConverterDZBToDZZ()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    DazzleEffectConverterDZBToDZZ::~DazzleEffectConverterDZBToDZZ()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool DazzleEffectConverterDZBToDZZ::_initialize()
    {
        m_convertExt = STRINGIZE_STRING_LOCAL( ".dzz" );

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzleEffectConverterDZBToDZZ::_finalize()
    {
        m_archivator = nullptr;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool DazzleEffectConverterDZBToDZZ::convert()
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
            , Helper::getContentFullPath( m_options.inputContent )
        );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_devFileGroup, full_outputFilePath, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open '%s'"
            , Helper::getContentFullPath( m_options.outputContent )
        );

        if( Helper::writeStreamArchiveData( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_DZZ ), GET_MAGIC_VERSION( MAGIC_DZZ ), false, data_memory, data_size, EAC_BEST ) == false )
        {
            LOGGER_ERROR( "dazzle converter invalid write '%s'"
                , Helper::getContentFullPath( m_options.outputContent )
            );

            return false;
        }

        if( Helper::closeOutputStreamFile( m_devFileGroup, stream ) == false )
        {
            LOGGER_ERROR( "dazzle converter invalid close '%s'"
                , Helper::getContentFullPath( m_options.outputContent )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzleEffectConverterDZBToDZZ::validateVersion( const InputStreamInterfacePtr & _stream ) const
    {
        if( Helper::loadStreamMagicHeader( _stream, GET_MAGIC_NUMBER( MAGIC_DZZ ), GET_MAGIC_VERSION( MAGIC_DZZ ) ) == false )
        {
            LOGGER_ERROR( "dazzle converter invalid magic header" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}