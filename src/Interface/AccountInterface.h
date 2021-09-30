#pragma once

#include "Interface/Interface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/OutputStreamInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Factorable.h"
#include "Kernel/FilePath.h"
#include "Kernel/Magic.h"

#ifndef MENGINE_ACCOUNT_SETTING_MAXVALUE
#define MENGINE_ACCOUNT_SETTING_MAXVALUE 1024
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DECLARE_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA, 'A', 'C', 'D', '1', 1 );
    //////////////////////////////////////////////////////////////////////////
    struct AccountUID
    {
        typedef Char value_type;
        static constexpr size_t size_data = 20;
        Char data[size_data];
    };
    //////////////////////////////////////////////////////////////////////////
    class AccountSettingProviderInterface
        : public Interface
    {
    public:
        virtual void onChangeSetting( const Char * _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AccountSettingProviderInterface> AccountSettingProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AccountInterface
        : public Interface
    {
    public:
        virtual const ConstString & getID() const = 0;
        virtual const FilePath & getFolder() const = 0;

    public:
        virtual void setUID( const AccountUID & _uid ) = 0;
        virtual const AccountUID & getUID() const = 0;

    public:
        virtual bool addSetting( const ConstString & _setting, const Char * _defaultValue, const AccountSettingProviderInterfacePtr & _provider ) = 0;
        virtual bool changeSetting( const ConstString & _setting, const Char * _value ) = 0;
        virtual bool getSetting( const ConstString & _setting, const Char ** const _value ) const = 0;
        virtual bool hasSetting( const ConstString & _setting, const Char ** const _value ) const = 0;

    public:
        virtual void apply() = 0;

    public:
        virtual bool load() = 0;
        virtual bool save() = 0;

    public:
        virtual InputStreamInterfacePtr openReadBinaryFile( const FilePath & _filePath ) = 0;
        virtual OutputStreamInterfacePtr openWriteBinaryFile( const FilePath & _filePath ) = 0;
        virtual bool closeReadBinaryFile( const InputStreamInterfacePtr & _stream ) = 0;
        virtual bool openWriteBinaryFile( const OutputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual MemoryInterfacePtr loadBinaryFile( const FilePath & _filePath ) = 0;
        virtual bool writeBinaryFile( const FilePath & _filePath, const void * _data, size_t _size ) = 0;
        virtual bool hasBinaryFile( const FilePath & _filePath ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AccountInterface> AccountInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}