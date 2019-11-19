#include "Document.h"

#include "Kernel/MemoryAllocator.h"

#include <string.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Document::Document()
        : m_buffer( nullptr )
    {
    }
    /////////////////////////////////////////////////////////////////////////
    Document::~Document()
    {
        Helper::freeMemory( m_buffer, "document" );
        m_buffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    Document::Document( const Document & _document )
    {
        const Char * msg = _document.c_str();

        this->operator =( msg );
    }
    //////////////////////////////////////////////////////////////////////////
    Document::Document( Document && _document )
        : m_buffer( _document.m_buffer )
    {
        _document.m_buffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Document::operator = ( const Char * _msg )
    {
        size_t len = ::strlen( _msg );
        m_buffer = (Char *)Helper::reallocateMemory( m_buffer, len + 1, "document" );
        ::strcpy( m_buffer, _msg );
    }
    //////////////////////////////////////////////////////////////////////////
    void Document::operator = ( const Document & _doc )
    {
        const Char * msg = _doc.c_str();

        this->operator =( msg );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Document::c_str() const
    {
        if( m_buffer == nullptr )
        {
            return "";
        }

        return m_buffer;
    }
}