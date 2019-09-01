#include "TextLocalePackage.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "stdex/xml_sax_parser.h"

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
    bool TextLocalePackage::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        m_fileGroup = _fileGroup;
        m_filePath = _filePath;

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( _fileGroup, _filePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, false, "invalid open file '%s:%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
        );

        size_t xml_buffer_size = stream->size();

        m_memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( m_memory, false );

        Char * memory_buffer = m_memory->newBuffer( xml_buffer_size + 1, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer, false );

        stream->read( memory_buffer, xml_buffer_size );
        memory_buffer[xml_buffer_size] = '\0';

        return true;
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
}