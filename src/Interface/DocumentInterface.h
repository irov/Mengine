#pragma once

#include "Kernel/Mixin.h"

#include "Config/Char.h"

#if defined(MENGINE_DOCUMENT_ENABLE)
#   define MENGINE_DOCUMENT_STR(Doc) ((Doc)->getMessage())
#else
#   define MENGINE_DOCUMENT_STR(Doc) MENGINE_STRING_EMPTY
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class DocumentInterface> DocumentInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class DocumentInterface
        : public Mixin
    {
    public:
        virtual void setParent( const DocumentInterfacePtr & _parent ) = 0;
        virtual const DocumentInterfacePtr & getParent() const = 0;

    public:
        virtual void setModulePath( const Char * _modulePath ) = 0;
        virtual const Char * getModulePath() const = 0;

        virtual void setFile( const Char * _file ) = 0;
        virtual const Char * getFile() const = 0;

        virtual void setLine( int32_t _line ) = 0;
        virtual int32_t getLine() const = 0;

        virtual void setFunction( const Char * _function ) = 0;
        virtual const Char * getFunction() const = 0;

    public:
        virtual void setMessage( const Char * _message ) = 0;
        virtual const Char * getMessage() const = 0;
        virtual size_t getMessageSize() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DocumentInterface> DocumentInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
