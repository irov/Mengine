#pragma once

#include "Interface/DocumentServiceInterface.h"

#include "Kernel/Win32Helper.h"

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
#   include "Kernel/Document.h"
#   include "Kernel/BufferHelper.h"

#   define MENGINE_DOCUMENT_FORWARD (this->getDocument())
#   define MENGINE_DOCUMENT_FUNCTION [](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( nullptr, Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "%s[%d]", _function, _line ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_MESSAGE(Format, ...) [=](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( nullptr, Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "%s[%u]: " Format, _function, _line, ##__VA_ARGS__ ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_FACTORABLE [this](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( dynamic_cast<const Mengine::Documentable *>(this)->getDocument(), Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "%s [==>] %s[%d]", dynamic_cast<const Mengine::Documentable *>(this)->getDocument()->getMessage(), _function, _line ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_FACTORABLE_PTR(Ptr) [Ptr](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( dynamic_cast<const Mengine::Documentable *>(Ptr)->getDocument(), Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "%s [==>] %s[%d]", dynamic_cast<const Mengine::Documentable *>(Ptr)->getDocument()->getMessage(), _function, _line ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_FACTORABLE_MEMBER(Ptr) [this](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = DOCUMENT_SERVICE()->createDocument( dynamic_cast<const Mengine::Documentable *>(Ptr)->getDocument(), Mengine::Helper::Win32GetCurrentDllPath(), _file, _function, _line, "%s [==>] %s[%d]", dynamic_cast<const Mengine::Documentable *>(Ptr)->getDocument()->getMessage(), _function, _line ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#else
#   define MENGINE_DOCUMENT_FORWARD nullptr
#   define MENGINE_DOCUMENT_FUNCTION nullptr
#   define MENGINE_DOCUMENT_MESSAGE(Format, ...) nullptr
#   define MENGINE_DOCUMENT_FACTORABLE nullptr
#   define MENGINE_DOCUMENT_FACTORABLE_PTR(Ptr) nullptr
#   define MENGINE_DOCUMENT_FACTORABLE_MEMBER(Ptr) nullptr
#endif
//////////////////////////////////////////////////////////////////////////