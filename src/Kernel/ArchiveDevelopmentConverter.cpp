#include "ArchiveDevelopmentConverter.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/FilePath.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/StreamHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ArchiveDevelopmentConverter::ArchiveDevelopmentConverter( const ConstString & _convertExt, const ArchivatorInterfacePtr & _archivator, magic_number_type _magicNumber, magic_version_type _magicVersion, bool _crc32, EArchivatorCompress _compress )
        : m_magicNumber( _magicNumber )
        , m_magicVersion( _magicVersion )
        , m_crc32( _crc32 )
        , m_compress( _compress )
        , m_archivator( _archivator )
    {
        m_convertExt = _convertExt;
    }
    //////////////////////////////////////////////////////////////////////////
    ArchiveDevelopmentConverter::~ArchiveDevelopmentConverter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchiveDevelopmentConverter::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ArchiveDevelopmentConverter::_finalize()
    {
        m_archivator = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchiveDevelopmentConverter::convert()
    {
        const FileGroupInterfacePtr & inputFileGroup = m_options.inputContent->getFileGroup();
        const FilePath & inputFilePath = m_options.inputContent->getFilePath();

        const FileGroupInterfacePtr & outputFileGroup = m_options.outputContent->getFileGroup();
        const FilePath & outputFilePath = m_options.outputContent->getFilePath();

        const FilePath & inputFolderPath = inputFileGroup->getFolderPath();
        const FilePath & outputFolderPath = outputFileGroup->getFolderPath();

        FilePath full_inputFilePath = Helper::concatenateFilePath( {inputFolderPath, inputFilePath} );
        FilePath full_outputFilePath = Helper::concatenateFilePath( {outputFolderPath, outputFilePath} );

        MemoryInterfacePtr data_cache = Helper::createMemoryCacheFile( m_fileGroupDev, full_inputFilePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data_cache, "invalid create cache memory '%s'"
            , Helper::getContentFullPath( m_options.inputContent ).c_str()
        );

        const void * data_memory = data_cache->getBuffer();
        size_t data_size = data_cache->getSize();

        MENGINE_ASSERTION_MEMORY_PANIC( data_memory, "invalid cache memory '%s'"
            , Helper::getContentFullPath( m_options.inputContent ).c_str()
        );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroupDev, full_outputFilePath, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open '%s'"
            , Helper::getContentFullPath( m_options.outputContent ).c_str()
        );

        bool successful = Helper::writeStreamArchiveMagic( stream, m_archivator, m_magicNumber, m_magicVersion, m_crc32, data_memory, data_size, m_compress );

        if( Helper::closeOutputStreamFile( m_fileGroupDev, stream ) == false )
        {
            LOGGER_ERROR( "archive converter invalid close '%s'"
                , Helper::getContentFullPath( m_options.outputContent ).c_str()
            );

            return false;
        }

        if( successful == false )
        {
            LOGGER_ERROR( "archive converter invalid write '%s'"
                , Helper::getContentFullPath( m_options.outputContent ).c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchiveDevelopmentConverter::validateVersion( const InputStreamInterfacePtr & _stream ) const
    {
        if( Helper::readStreamMagicHeader( _stream, m_magicNumber, m_magicVersion ) == false )
        {
            LOGGER_ERROR( "archive converter invalid magic header" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
