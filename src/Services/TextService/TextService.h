#pragma once

#include "Interface/TextServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "TextEntry.h"
#include "TextLocalePackage.h"
#include "TextArgumentValue.h"
#include "TextArgumentId.h"
#include "TextArgumentContext.h"

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
        bool loadTextEntry( const VectorConstString & _locales, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;
        bool unloadTextEntry( const VectorConstString & _locales, const ContentInterfacePtr & _content ) override;

    public:
        bool hasTextEntry( const ConstString & _textId, TextEntryInterfacePtr * const _entry ) const override;
        const TextEntryInterfacePtr & getTextEntry( const ConstString & _textId ) const override;

    public:
        void foreachTextEntry( const LambdaTextEntry & _lambda ) const override;

    public:
        void setTextAlias( const ConstString & _environment, const ConstString & _textId, const ConstString & _key ) override;
        void removeTextAlias( const ConstString & _environment, const ConstString & _textId ) override;
        bool isTextAlias( const ConstString & _textId ) const override;
        bool hasTextAlias( const ConstString & _environment, const ConstString & _textId ) const override;
        const ConstString & getTextAlias( const ConstString & _environment, const ConstString & _textId ) const override;

    public:
        void setTextAliasArguments( const ConstString & _environment, const ConstString & _textId, const VectorTextArguments & _arguments ) override;
        bool getTextAliasArguments( const ConstString & _environment, const ConstString & _textId, VectorTextArguments * const _arguments ) const override;
        bool hasTextAliasArguments( const ConstString & _environment, const ConstString & _textId ) const override;
        void removeTextAliasArguments( const ConstString & _environment, const ConstString & _textId ) override;

    public:
        TextEntryInterfacePtr createTextEntry( const ConstString & _textId, const TextEntryDesc & _desc, const DocumentInterfacePtr & _doc ) override;

    public:
        bool addTextEntry( const ConstString & _textId, const TextEntryDesc & _desc, bool _isOverride, bool * const _isDublicate, const DocumentInterfacePtr & _doc ) override;
        bool removeTextEntry( const ConstString & _textId ) override;
        void removeTextEntries( const Tags & _tag ) override;

    protected:
        TextArgumentInterfacePtr createTextArgumentValue( const String & _value, const DocumentInterfacePtr & _doc ) override;
        TextArgumentInterfacePtr createTextArgumentId( const ConstString & _textId, const DocumentInterfacePtr & _doc ) override;
        TextArgumentInterfacePtr createTextArgumentContext( const LambdaTextArgumentContext & _context, const DocumentInterfacePtr & _doc ) override;

    protected:
        const VectorU32String & getLineDelims() const override;

    protected:
        typedef Hashtable<ConstString, TextEntryInterfacePtr> HashtableTextEntry;
        HashtableTextEntry m_texts;

        typedef Pair<ConstString, ConstString> PairAliasKey;
        typedef Map<PairAliasKey, ConstString> MapTextAliases;
        MapTextAliases m_aliases;

        typedef Map<PairAliasKey, VectorTextArguments> MapTextAliasesArguments;
        MapTextAliasesArguments m_aliasesArguments;

        typedef Vector<TextLocalePackagePtr> VectorTextLocalePackages;
        VectorTextLocalePackages m_packages;

        FontInterfacePtr m_defaultFont;

        FactoryInterfacePtr m_factoryTextEntry;
        FactoryInterfacePtr m_factoryTextLocalePackage;
        FactoryInterfacePtr m_factoryTextArgumentValue;
        FactoryInterfacePtr m_factoryTextArgumentId;
        FactoryInterfacePtr m_factoryTextArgumentContext;

        VectorU32String m_lineDelims;

    protected:
        class TextManagerLoadSaxCallback;
    };
}
