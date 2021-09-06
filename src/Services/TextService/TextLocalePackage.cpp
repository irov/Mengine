#include "TextLocalePackage.h"

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextLocalePackage::TextLocalePackage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextLocalePackage::~TextLocalePackage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextLocalePackage::initialize( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        m_locale = _locale;
        m_fileGroup = _fileGroup;
        m_filePath = _filePath;

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open file '%s'"
            , Helper::getFileGroupFullPath( _fileGroup, _filePath )
        );

        size_t xml_buffer_size = stream->size();

        m_memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( m_memory );

        Char * memory_buffer = m_memory->newBuffer( xml_buffer_size + 1 );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer );

        stream->read( memory_buffer, xml_buffer_size );
        memory_buffer[xml_buffer_size] = '\0';

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TextLocalePackage::finalize()
    {
        m_fileGroup = nullptr;
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & TextLocalePackage::getFileGroup() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & TextLocalePackage::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr TextLocalePackage::getXmlBuffer() const
    {
        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
}