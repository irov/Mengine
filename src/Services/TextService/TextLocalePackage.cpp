#include "TextLocalePackage.h"

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ContentHelper.h"

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
    bool TextLocalePackage::initialize( const VectorConstString & _locales, const ContentInterfacePtr & _content )
    {
        m_locales = _locales;
        m_content = _content;

        InputStreamInterfacePtr stream = m_content->openInputStreamFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open file '%s'"
            , Helper::getContentFullPath( m_content )
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
        m_content = nullptr;
        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & TextLocalePackage::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr TextLocalePackage::getXmlBuffer() const
    {
        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
}