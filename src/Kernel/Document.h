#pragma once

#include "Config/Config.h"
#include "Config/String.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Document> DocumentPtr;
    //////////////////////////////////////////////////////////////////////////
    class Document
        : public Factorable
    {
    public:
        Document( const Char * _file, const Char * _function, uint32_t _line );
        ~Document();

    public:
        const Char * getModulePath() const;
        const Char * getFile() const;
        const Char * getFunction() const;
        uint32_t getLine() const;

    public:
        void setParent( const DocumentPtr & _parent );
        const DocumentPtr & getParent() const;

    public:
        void message( const char * _format, ... );

    public:
        const Char * c_str() const;

    protected:
        DocumentPtr m_parent;

        Char m_modulePath[MENGINE_MAX_PATH] = {'\0'};

        const Char * m_file;
        const Char * m_function;
        uint32_t m_line;
        String m_message;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Document> DocumentPtr;
    //////////////////////////////////////////////////////////////////////////
}

#ifdef MENGINE_DEBUG
#   define MENGINE_DOCUMENT_MESSAGE(Doc) Doc->c_str()
#else
#   define MENGINE_DOCUMENT_MESSAGE(Doc) MENGINE_STRING_EMPTY
#endif