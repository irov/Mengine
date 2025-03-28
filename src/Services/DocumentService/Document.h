#pragma once

#include "Interface/DocumentInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/String.h"
#include "Kernel/PathString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Document
        : public DocumentInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( Document );

    public:
        Document();
        ~Document() override;

    public:
        void setParent( const DocumentInterfacePtr & _parent ) override;
        const DocumentInterfacePtr & getParent() const override;

    public:
        void setModulePath( const Char * _modulePath ) override;
        const Char * getModulePath() const override;

        void setFile( const Char * _file ) override;
        const Char * getFile() const override;

        void setLine( int32_t _line ) override;
        int32_t getLine() const override;

        void setFunction( const Char * _function ) override;
        const Char * getFunction() const override;

    public:
        void setMessage( const Char * _message ) override;
        const Char * getMessage() const override;
        size_t getMessageSize() const override;

    protected:
        DocumentInterfacePtr m_parent;

        PathString m_modulePath;

        const Char * m_file;
        const Char * m_function;
        int32_t m_line;

        String m_message;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Document, DocumentInterface> DocumentPtr;
    //////////////////////////////////////////////////////////////////////////
}
