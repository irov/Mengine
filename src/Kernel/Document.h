#pragma once

#include "Config/Config.h"
#include "Config/Char.h"

namespace Mengine
{
    class Document
    {
    public:
        Document();
        ~Document();

    public:
        Document( const Document & _document );
        Document( Document && _document );

    public:
        void operator = ( const Char * _msg );

    public:
        const Char * c_str() const;

    protected:
        Char * m_buffer;
    };
}
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
#   include "Kernel/BufferHelper.h"

#   define MENGINE_DOCUMENT_FUNCTION [](const Mengine::Char * _function, uint32_t _line) -> const Mengine::Char * { static Mengine::Char buffer[4096] = {0}; Mengine::Helper::bufferFormat( buffer, 4096, "%s[%d]", _function, _line ); return buffer;}(MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT(Format, ...) [=](const Mengine::Char * _function, uint32_t _line) -> const Mengine::Char * { static Mengine::Char buffer[4096] = {0}; Mengine::Helper::bufferFormat( buffer, 4096, "%s[%u]: " Format, _function, _line, __VA_ARGS__ ); return buffer;}(MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_FACTORABLE [this](const Mengine::Char * _doc) -> const Mengine::Char * { static Mengine::Char buffer[4096] = {0}; Mengine::Helper::bufferFormat( buffer, 4096, "%s [==>] '%s'", this->getDocument(), _doc ); return buffer;}(MENGINE_DOCUMENT_FUNCTION)
#   define MENGINE_DOCUMENT_FACTORABLE_PTR(Ptr) [Ptr](const Mengine::Char * _doc) -> const Mengine::Char * { static Mengine::Char buffer[4096] = {0}; Mengine::Helper::bufferFormat( buffer, 4096, "%s [==>] '%s'", Ptr->getDocument(), _doc ); return buffer;}(MENGINE_DOCUMENT_FUNCTION)
#else
#   define MENGINE_DOCUMENT_FUNCTION MENGINE_STRING_EMPTY
#   define MENGINE_DOCUMENT(Format, ...) MENGINE_STRING_EMPTY
#   define MENGINE_DOCUMENT_FACTORABLE MENGINE_STRING_EMPTY
#   define MENGINE_DOCUMENT_FACTORABLE_PTR(Ptr) MENGINE_STRING_EMPTY
#endif
//////////////////////////////////////////////////////////////////////////