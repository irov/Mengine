#pragma once

#include "Interface/InputStreamInterface.h"
#include "Interface/OutputStreamInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Factorable.h"
#include "Kernel/FilePath.h"
#include "Kernel/Magic.h"
#include "Kernel/Mixin.h"

#include "Config/String.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
    DECLARE_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA, 'A', 'C', 'D', '1', 1 );
	//////////////////////////////////////////////////////////////////////////
	class AccountSettingProviderInterface
		: public Mixin
	{
	public:
		virtual void onChangeSetting( const WString& _value ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<AccountSettingProviderInterface> AccountSettingProviderInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
    class AccountInterface
        : public Mixin
    {
    public:
        virtual const ConstString & getID() const = 0;
        virtual const FilePath & getFolder() const = 0;

    public:
        virtual void setUID( const String & _uid ) = 0;
        virtual const String & getUID() const = 0;

    public:
        virtual bool addSetting( const ConstString & _setting, const WString& _defaultValue, const AccountSettingProviderInterfacePtr & _provider ) = 0;
        virtual bool changeSetting( const ConstString & _setting, const WString& _value ) = 0;
        virtual WString getSetting( const ConstString & _setting ) const = 0;
        virtual bool hasSetting( const ConstString & _setting ) const = 0;

    public:
        virtual void apply() = 0;

        virtual bool load() = 0;
        virtual bool save() = 0;

    public:
        virtual InputStreamInterfacePtr openReadBinaryFile( const FilePath & _fileName ) = 0;
        virtual OutputStreamInterfacePtr openWriteBinaryFile( const FilePath & _fileName ) = 0;

    public:
        virtual MemoryInterfacePtr loadBinaryFile( const FilePath & _filename ) = 0;
        virtual bool writeBinaryFile( const FilePath & _filename, const void * _data, size_t _size ) = 0;
        virtual bool hasBinaryFile( const FilePath & _filename ) const = 0;
    };
	//////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AccountInterface> AccountInterfacePtr;
	//////////////////////////////////////////////////////////////////////////    
}