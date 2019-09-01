#pragma once

#include "Interface/TextServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "TextLocalePackage.h"

#include "TextEntry.h"

#include "Kernel/Factory.h"
#include "Kernel/Hashtable.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHolderLocalString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Pool.h"

#include "Config/Pair.h"
#include "Config/Map.h"
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
        bool loadTextEntry( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;
        bool unloadTextEntry( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;

    public:
        TextFontInterfacePtr createFont( const ConstString & _fontName, const ConstString & _fontType, const Char * _doc ) override;
        bool loadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;
        bool unloadFonts( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;

    public:
        bool hasTextEntry( const ConstString & _key, TextEntryInterfacePtr * _entry ) const override;
        const TextEntryInterfacePtr & getTextEntry( const ConstString & _key ) const override;

    public:
        bool existFont( const ConstString & _name, TextFontInterfacePtr * _font ) const override;
        const TextFontInterfacePtr & getFont( const ConstString & _name ) const override;

    public:
        void setTextAlias( const ConstString & _environment, const ConstString & _alias, const ConstString & _key ) override;
        void removeTextAlias( const ConstString & _environment, const ConstString & _alias ) override;
        bool hasTextAlias( const ConstString & _environment, const ConstString & _alias ) const override;
        const ConstString & getTextAlias( const ConstString & _environment, const ConstString & _alias ) const override;

    public:
        void setTextAliasArguments( const ConstString & _environment, const ConstString & _alias, const VectorString & _arguments ) override;
        void removeTextAliasArguments( const ConstString & _environment, const ConstString & _alias ) override;
        bool getTextAliasArguments( const ConstString & _environment, const ConstString & _alias, VectorString & _arguments ) const override;

    public:
        void foreachFonts( const LambdaTextFont & _lambda ) override;

    public:
        bool validate() const override;

    public:
        const ConstString & getDefaultFontName() const override;

    public:
        bool addTextEntry( const ConstString & _key
            , const Char * _text
            , size_t _size
            , const ConstString & _font
            , const Color & _colorFont
            , float _lineOffset
            , float _charOffset
            , float _maxLength
            , ETextHorizontAlign _horizontAlign
            , ETextVerticalAlign _verticalAlign
            , float _scale
            , uint32_t _params
            , bool _isOverride ) override;

        bool removeTextEntry( const ConstString & _key );

    public:
        bool directFontCompile( const ConstString & _name ) override;
        bool directFontRelease( const ConstString & _name ) override;

    protected:
        typedef Hashtable<ConstString, TextEntryInterfacePtr> HashtableTextEntry;
        HashtableTextEntry m_texts;

        typedef Hashtable<ConstString, TextFontInterfacePtr> HashtableTextFont;
        HashtableTextFont m_fonts;

        typedef Pair<ConstString, ConstString> PairAliasKey;

        typedef Map<PairAliasKey, ConstString> MapTextAliases;
        MapTextAliases m_aliases;

        typedef Map<PairAliasKey, VectorString> MapTextAliasesArguments;
        MapTextAliasesArguments m_aliasesArguments;

        typedef Vector<TextLocalePackagePtr> VectorTextLocalePackages;
        VectorTextLocalePackages m_packages;

        ConstString m_defaultFontName;

        FactoryPtr m_factoryTextEntry;
        FactoryPtr m_factoryTextLocalePackage;

        typedef stdex::template_pool<ConstStringHolderLocalString, 1024> PoolConstStringHolderLocalString;
        PoolConstStringHolderLocalString m_poolLocalString;

    protected:
        class TextManagerLoadSaxCallback;
        class TextManagerUnloadSaxCallback;

        void createLocalString_( const Char * _text, ConstString::size_type _size, ConstString & _cstr );
    };
}
