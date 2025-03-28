#pragma once

#include "Config/Config.h"

#if defined(MENGINE_DOCUMENT_ENABLE)
#   include "Interface/DocumentServiceInterface.h"

#   include "Kernel/BufferHelper.h"

#   include "Config/DynamicCast.h"
#   include "Config/StdInt.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const DocumentInterfacePtr & getDocumentableFromPtr( const T * _ptr )
        {
            const Documentable * documentable = Helper::dynamicCast<const Documentable *>( _ptr );

            if( documentable == nullptr )
            {
                return DocumentInterfacePtr::none();
            }

            const DocumentInterfacePtr & document = documentable->getDocument();

            return document;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const DocumentInterfacePtr & getDocumentableFromPtr( const IntrusivePtr<T> & _ptr )
        {
            const T * ptr_get = _ptr.get();

            const Documentable * documentable = Helper::dynamicCast<const Documentable *>( ptr_get );

            if( documentable == nullptr )
            {
                return DocumentInterfacePtr::none();
            }

            const DocumentInterfacePtr & document = documentable->getDocument();

            return document;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const Char * getDocumentableMessageFromPtr( const T * _ptr )
        {
            const Documentable * documentable = Helper::dynamicCast<const Documentable *>( _ptr );

            if( documentable == nullptr )
            {
                return "[None Documentable]";
            }

            const DocumentInterfacePtr & document = documentable->getDocument();

            if( document == nullptr )
            {
                return "[None Document]";
            }

            const Char * message = document->getMessage();

            return message;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const Char * getDocumentableMessageFromPtr( const IntrusivePtr<T> & _ptr )
        {
            const T * ptr_get = _ptr.get();

            const Documentable * documentable = Helper::dynamicCast<const Documentable *>( ptr_get );

            if( documentable == nullptr )
            {
                return "[None Documentable]";
            }

            const DocumentInterfacePtr & document = documentable->getDocument();

            if( document == nullptr )
            {
                return "[None Document]";
            }

            const Char * message = document->getMessage();

            return message;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_DOCUMENT_FORWARD (this->getDocument())
#   define MENGINE_DOCUMENT_FORWARD_PTR(Ptr) (Ptr->getDocument())
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_DOCUMENT_FUNCTION [](const Mengine::Char * _file, int32_t _line, const Mengine::Char * _function) { \
        Mengine::DocumentInterfacePtr doc = DOCUMENT_SERVICE()->createDocument( nullptr, MENGINE_CODE_LIBRARY, _file, _line, _function, "%s[%d]", _function, _line ); \
        return doc; \
    }(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_DOCUMENT_MESSAGE(Format, ...) [=](const Mengine::Char * _file, int32_t _line, const Mengine::Char * _function) { \
        Mengine::DocumentInterfacePtr doc = DOCUMENT_SERVICE()->createDocument( nullptr, MENGINE_CODE_LIBRARY, _file, _line, _function, "%s[%d]: " Format, _function, _line, ##__VA_ARGS__ ); \
        return doc; \
    }(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_DOCUMENT_FACTORABLE [this](const Mengine::Char * _file, int32_t _line, const Mengine::Char * _function) { \
        Mengine::DocumentInterfacePtr doc = DOCUMENT_SERVICE()->createDocument( Detail::getDocumentableFromPtr(this), MENGINE_CODE_LIBRARY, _file, _line, _function, "%s [==>] %s[%d]", Detail::getDocumentableMessageFromPtr(this), _function, _line ); \
        return doc; \
    }(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_DOCUMENT_FACTORABLE_PTR(Ptr) [Ptr](const Mengine::Char * _file, int32_t _line, const Mengine::Char * _function) { \
        Mengine::DocumentInterfacePtr doc = DOCUMENT_SERVICE()->createDocument( Detail::getDocumentableFromPtr(Ptr), MENGINE_CODE_LIBRARY, _file, _line, _function, "%s [==>] %s[%d]", Detail::getDocumentableMessageFromPtr(Ptr), _function, _line ); \
        return doc; \
    }(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_DOCUMENT_FACTORABLE_MEMBER(Ptr) [Ptr](const Mengine::Char * _file, int32_t _line, const Mengine::Char * _function) { \
        Mengine::DocumentInterfacePtr doc = DOCUMENT_SERVICE()->createDocument( Detail::getDocumentableFromPtr(Ptr), MENGINE_CODE_LIBRARY, _file, _line, _function, "%s [==>] %s[%d]", Detail::getDocumentableMessageFromPtr(Ptr), _function, _line ); \
        return doc; \
    }(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
    //////////////////////////////////////////////////////////////////////////
#else
#   include "Interface/DocumentInterface.h"

#   define MENGINE_DOCUMENT_FORWARD (Mengine::DocumentInterfacePtr::none())
#   define MENGINE_DOCUMENT_FORWARD_PTR(Ptr) (Mengine::DocumentInterfacePtr::none())
#   define MENGINE_DOCUMENT_FUNCTION (Mengine::DocumentInterfacePtr::none())
#   define MENGINE_DOCUMENT_MESSAGE(Format, ...) (Mengine::DocumentInterfacePtr::none())
#   define MENGINE_DOCUMENT_FACTORABLE (Mengine::DocumentInterfacePtr::none())
#   define MENGINE_DOCUMENT_FACTORABLE_PTR(Ptr) (Mengine::DocumentInterfacePtr::none())
#   define MENGINE_DOCUMENT_FACTORABLE_MEMBER(Ptr) (Mengine::DocumentInterfacePtr::none())
#endif
