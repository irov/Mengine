#include "TextLocalePak.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "stdex/xml_sax_parser.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextLocalePack::TextLocalePack()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextLocalePack::~TextLocalePack()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextLocalePack::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _path )
    {
        m_fileGroup = _fileGroup;
        m_path = _path;

        InputStreamInterfacePtr xml_text = FILE_SERVICE()
            ->openInputFile( _fileGroup, _path, false, MENGINE_DOCUMENT_FUNCTION );

        if( xml_text == nullptr )
        {
            LOGGER_ERROR( "invalid open file %s:%s"
                , _fileGroup->getName().c_str()
                , _path.c_str()
            );

            return false;
        }

        size_t xml_buffer_size = xml_text->size();

        m_memory = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        Char * memory_buffer = m_memory->newBuffer( xml_buffer_size + 1, MENGINE_DOCUMENT_FUNCTION );

        xml_text->read( memory_buffer, xml_buffer_size );
        memory_buffer[xml_buffer_size] = '\0';

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & TextLocalePack::getFileGroup() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & TextLocalePack::getPath() const
    {
        return m_path;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr TextLocalePack::getXmlBuffer() const
    {
        return m_memory;
    }
}