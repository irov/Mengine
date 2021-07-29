#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/TextEntryInterface.h"

#include "Kernel/Tags.h"

namespace Mengine
{
    class TextServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TextService" )

    public:
        virtual bool loadTextEntry( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) = 0;
        virtual bool unloadTextEntry( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) = 0;

    public:
        virtual TextEntryInterfacePtr createTextEntry( const ConstString & _key
            , const Char * _text
            , size_t _size
            , const Tags & _tags
            , const TextFontInterfacePtr & _font
            , const Color & _colorFont
            , float _lineOffset
            , float _charOffset
            , float _maxLength
            , ETextHorizontAlign _horizontAlign
            , ETextVerticalAlign _verticalAlign
            , float _scale
            , uint32_t _params
            , const DocumentPtr & _doc ) = 0;

    public:
        virtual bool addTextEntry( const ConstString & _key
            , const Char * _text
            , size_t _size
            , const Tags & _tags
            , const TextFontInterfacePtr & _font
            , const Color & _colorFont
            , float _lineOffset
            , float _charOffset
            , float _maxLength
            , ETextHorizontAlign _horizontAlign
            , ETextVerticalAlign _verticalAlign
            , float _scale
            , uint32_t _params
            , bool _isOverride
            , bool * const _isDublicate
            , const DocumentPtr & _doc ) = 0;

        virtual bool removeTextEntry( const ConstString & _key ) = 0;
        virtual void removeTextEntries( const Tags & _tag ) = 0;

    public:
        virtual bool hasTextEntry( const ConstString & _key, TextEntryInterfacePtr * const _entry ) const = 0;
        virtual const TextEntryInterfacePtr & getTextEntry( const ConstString & _key ) const = 0;

    public:
        virtual void setTextAlias( const ConstString & _environment, const ConstString & _alias, const ConstString & _key ) = 0;
        virtual void removeTextAlias( const ConstString & _environment, const ConstString & _alias ) = 0;
        virtual bool isTextAlias( const ConstString & _alias ) const = 0;
        virtual bool hasTextAlias( const ConstString & _environment, const ConstString & _alias ) const = 0;
        virtual const ConstString & getTextAlias( const ConstString & _environment, const ConstString & _alias ) const = 0;

    public:
        virtual void setTextAliasArguments( const ConstString & _environment, const ConstString & _alias, const VectorString & _arguments ) = 0;
        virtual void removeTextAliasArguments( const ConstString & _environment, const ConstString & _alias ) = 0;
        virtual bool getTextAliasArguments( const ConstString & _environment, const ConstString & _alias, VectorString * const _arguments ) const = 0;

    public:
        virtual bool validate() const = 0;

    public:
        virtual const VectorU32String & getLineDelims() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TEXT_SERVICE()\
    ((Mengine::TextServiceInterface*)SERVICE_GET(Mengine::TextServiceInterface))
//////////////////////////////////////////////////////////////////////////