#pragma once

#include "Config/Config.h"

//////////////////////////////////////////////////////////////////////////
#if MENGINE_DOCUMENT_ENABLE
#   include "Interface/DocumentServiceInterface.h"

#   include "Kernel/Document.h"
#   include "Kernel/BufferHelper.h"
#   include "Kernel/Win32Helper.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const DocumentPtr & getDocumentableFromPtr( const T * _ptr )
        {
            const Documentable * documentable = dynamic_cast<const Documentable *>(_ptr);

            const DocumentPtr & document = documentable->getDocument();

            return document;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        const DocumentPtr & getDocumentableFromPtr( const IntrusivePtr<T> & _ptr )
        {
            const T * ptr_get = _ptr.get();

            const Documentable * documentable = dynamic_cast<const Documentable *>(ptr_get);

            const DocumentPtr & document = documentable->getDocument();

            return document;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}

#   define MENGINE_DOCUMENT_FORWARD (this->getDocument())
#   define MENGINE_DOCUMENT_FORWARD_PTR(Ptr) (Ptr->getDocument())
#   define MENGINE_DOCUMENT_FUNCTION [](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( nullptr, Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "%s[%d]", _function, _line ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_MESSAGE(Format, ...) [=](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( nullptr, Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "%s[%u]: " Format, _function, _line, ##__VA_ARGS__ ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_FACTORABLE [this](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( Detail::getDocumentableFromPtr(this), Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "%s [==>] %s[%d]", Detail::getDocumentableFromPtr(this)->getMessage(), _function, _line ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_FACTORABLE_PTR(Ptr) [Ptr](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( Detail::getDocumentableFromPtr(Ptr), Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "%s [==>] %s[%d]", Detail::getDocumentableFromPtr(Ptr)->getMessage(), _function, _line ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_FACTORABLE_MEMBER(Ptr) [this](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( Detail::getDocumentableFromPtr(Ptr), Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "%s [==>] %s[%d]", Detail::getDocumentableFromPtr(Ptr)->getMessage(), _function, _line ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#else
#   define MENGINE_DOCUMENT_FORWARD nullptr
#   define MENGINE_DOCUMENT_FORWARD_PTR(Ptr) nullptr
#   define MENGINE_DOCUMENT_FUNCTION nullptr
#   define MENGINE_DOCUMENT_MESSAGE(Format, ...) nullptr
#   define MENGINE_DOCUMENT_FACTORABLE nullptr
#   define MENGINE_DOCUMENT_FACTORABLE_PTR(Ptr) nullptr
#   define MENGINE_DOCUMENT_FACTORABLE_MEMBER(Ptr) nullptr
#endif
//////////////////////////////////////////////////////////////////////////