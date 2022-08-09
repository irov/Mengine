#pragma once

#include "Interface/TextServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "TextLocalePackage.h"
#include "TextEntry.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"
#include "Kernel/Tags.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHolderLocalString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Pool.h"
#include "Kernel/Pair.h"
#include "Kernel/Map.h"
#include "Kernel/IntrusiveList.h"

#include "Config/Typedef.h"

namespace Mengine
{
    class TextService
        : public ServiceBase<TextServiceInterface>
    {
    public:
        TextService();
        ~TextService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool loadTextEntry( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;
        bool unloadTextEntry( const ConstString & _locale, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;

    public:
        bool hasTextEntry( const ConstString & _textId, TextEntryInterfacePtr * const _entry ) const override;
        const TextEntryInterfacePtr & getTextEntry( const ConstString & _textId ) const override;

    public:
        void foreachTextEntry( const LambdaTextEntry & _lambda ) const override;

    public:
        void setTextAlias( const ConstString & _environment, const ConstString & _alias, const ConstString & _key ) override;
        void removeTextAlias( const ConstString & _environment, const ConstString & _alias ) override;
        bool isTextAlias( const ConstString & _alias ) const override;
        bool hasTextAlias( const ConstString & _environment, const ConstString & _alias ) const override;
        const ConstString & getTextAlias( const ConstString & _environment, const ConstString & _alias ) const override;

    public:
        void setTextAliasArguments( const ConstString & _environment, const ConstString & _alias, const VectorString & _arguments ) override;
        void removeTextAliasArguments( const ConstString & _environment, const ConstString & _alias ) override;
        bool getTextAliasArguments( const ConstString & _environment, const ConstString & _alias, VectorString * const _arguments ) const override;

    public:
        TextEntryInterfacePtr createTextEntry( const ConstString & _textId
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
            , const DocumentPtr & _doc ) override;

    public:
        bool addTextEntry( const ConstString & _textId
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
            , const DocumentPtr & _doc ) override;

        bool removeTextEntry( const ConstString & _textId ) override;
        void removeTextEntries( const Tags & _tag ) override;

    protected:
        const VectorU32String & getLineDelims() const override;

    protected:
        typedef Hashtable<ConstString, TextEntryPtr> HashtableTextEntry;
        HashtableTextEntry m_texts;

        typedef Pair<ConstString, ConstString> PairAliasKey;
        typedef Map<PairAliasKey, ConstString> MapTextAliases;
        MapTextAliases m_aliases;

        typedef Map<PairAliasKey, VectorString> MapTextAliasesArguments;
        MapTextAliasesArguments m_aliasesArguments;

        typedef Vector<TextLocalePackagePtr> VectorTextLocalePackages;
        VectorTextLocalePackages m_packages;

        TextFontInterfacePtr m_defaultFont;

        FactoryInterfacePtr m_factoryTextEntry;
        FactoryInterfacePtr m_factoryTextLocalePackage;

        VectorU32String m_lineDelims;

    protected:
        class TextManagerLoadSaxCallback;
    };
}
