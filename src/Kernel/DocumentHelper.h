#pragma once

#include "Kernel/Document.h"
#include "Kernel/FactorableUnique.h"

#ifdef MENGINE_DEBUG
#   include "Kernel/BufferHelper.h"

#   define MENGINE_DOCUMENT_FUNCTION [](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = Mengine::Helper::makeFactorableUnique<Mengine::Document>(_file, _function, _line); doc->message( "%s[%d]", _function, _line ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT(Format, ...) [=](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = Mengine::Helper::makeFactorableUnique<Mengine::Document>(_file, _function, _line); ; doc->message( "%s[%u]: " Format, _function, _line, __VA_ARGS__ ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_FACTORABLE [this](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = Mengine::Helper::makeFactorableUnique<Mengine::Document>(_file, _function, _line); doc->message( "%s [==>] %s[%d]", this->getDocument()->c_str(), _function, _line ); doc->setParent( this->getDocument() ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#   define MENGINE_DOCUMENT_FACTORABLE_PTR(Ptr) [Ptr](const Mengine::Char * _file, const Mengine::Char * _function, uint32_t _line) { Mengine::DocumentPtr doc = Mengine::Helper::makeFactorableUnique<Mengine::Document>(_file, _function, _line); doc->message( "%s [==>] %s[%d]", Ptr->getDocument()->c_str(), _function, _line ); doc->setParent( Ptr->getDocument() ); return doc;}(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#else
#   define MENGINE_DOCUMENT_FUNCTION nullptr
#   define MENGINE_DOCUMENT(Format, ...) nullptr
#   define MENGINE_DOCUMENT_FACTORABLE nullptr
#   define MENGINE_DOCUMENT_FACTORABLE_PTR(Ptr) nullptr
#endif