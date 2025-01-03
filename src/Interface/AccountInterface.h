#pragma once

#include "Interface/AccountSettingProviderInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/OutputStreamInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/AccountUID.h"
#include "Kernel/ConstString.h"
#include "Kernel/Factorable.h"
#include "Kernel/FilePath.h"
#include "Kernel/Magic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DECLARE_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA, 'A', 'C', 'D', '1', 1 );
    //////////////////////////////////////////////////////////////////////////
    class AccountInterface
        : public Mixin
    {
    public:
        virtual const ConstString & getId() const = 0;
        virtual const FilePath & getFolderName() const = 0;

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