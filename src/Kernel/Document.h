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
        Document();
        ~Document();

    public:
        void setParent( const DocumentPtr & _parent );
        const DocumentPtr & getParent() const;

    public:
        void setModulePath( const Char * _modulePath );
        const Char * getModulePath() const;

        void setFile( const Char * _file );
        const Char * getFile() const;

        void setFunction( const Char * _function );
        const Char * getFunction() const;

        void setLine( uint32_t _line );
        uint32_t getLine() const;

    public:
        void setMessage( const Char * _message );
        const Char * getMessage() const;

    protected:
        DocumentPtr m_parent;

        String m_modulePath;

        const Char * m_file;
        const Char * m_function;
        uint32_t m_line;

        String m_message;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Document> DocumentPtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
#   define MENGINE_DOCUMENT_STR(Doc) Doc->getMessage()
#else
#   define MENGINE_DOCUMENT_STR(Doc) MENGINE_STRING_EMPTY
#endif