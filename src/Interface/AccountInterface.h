#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/StreamInterface.h"
#   include "Interface/MemoryInterface.h"

#   include "Core/ConstString.h"
#	include "Factory/FactorablePtr.h"

#   include "Config/String.h"

#	include "Core/FilePath.h"
#	include "Core/Magic.h"

#	include "pybind/object.hpp"

namespace Menge
{
	DECLARE_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA, 'A', 'C', 'D', '1', 1 );

    class AccountInterface
		: public ServantInterface
    {
    public:
        virtual const ConstString & getID() const = 0;
        virtual const FilePath & getFolder() const = 0;

	public:
		virtual void setUID( const String & _uid ) = 0;
		virtual const String & getUID() const = 0;

    public:
		virtual bool addSetting( const ConstString & _setting, const WString& _defaultValue, const pybind::object & _applyFunc ) = 0;
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

	typedef stdex::intrusive_ptr<AccountInterface> AccountInterfacePtr;

    class AccountProviderInterface
		: public ServantInterface
    {
	public:
		AccountProviderInterface(){};
		virtual ~AccountProviderInterface(){};

    public:
        virtual void onCreateAccount( const ConstString & _accountID, bool _global ) = 0;
        virtual void onDeleteAccount( const ConstString & _accountID ) = 0;
        virtual void onSelectAccount( const ConstString & _accountID ) = 0;
        virtual void onUnselectAccount( const ConstString & _accountID ) = 0;
    };

	typedef stdex::intrusive_ptr<AccountProviderInterface> AccountProviderInterfacePtr;

	class AccountVisitorInterface
	{
	public:
		virtual void onAccount( const AccountInterfacePtr & _account ) = 0;
	};

    class AccountServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("AccountService")

	public:
		virtual void setAccountProviderInterface(const AccountProviderInterfacePtr & _accountProvider ) = 0;

    public:
        virtual AccountInterfacePtr createAccount() = 0;
		virtual AccountInterfacePtr createGlobalAccount() = 0;

    public:
        virtual void deleteAccount( const ConstString& _accountID ) = 0;
        virtual bool selectAccount( const ConstString& _accountID ) = 0;

    public:
        virtual bool loadAccounts() = 0;
        virtual bool saveAccounts() = 0;

    public:
        virtual void setDefaultAccount( const ConstString & _accountID ) = 0;
        virtual const ConstString & getDefaultAccountID() const = 0;
		virtual bool isCurrentDefaultAccount() const = 0;

        virtual bool hasDefaultAccount() const = 0;

        virtual bool selectDefaultAccount() = 0;

	public:
		virtual void setGlobalAccount( const ConstString & _accountID ) = 0;
		virtual const ConstString & getGlobalAccountID() const = 0;

		virtual bool hasGlobalAccount() const = 0;

    public:
        virtual bool hasCurrentAccount() const = 0;
        virtual const ConstString & getCurrentAccountID() const = 0;

	public:
        virtual AccountInterfacePtr getAccount( const ConstString& _accountID ) = 0;

	public:
        virtual void visitAccounts( AccountVisitorInterface * _visitor ) const = 0;
    };

#   define ACCOUNT_SERVICE()\
    ((AccountServiceInterface*)SERVICE_GET(Menge::AccountServiceInterface))
}