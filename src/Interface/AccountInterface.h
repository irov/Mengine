#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/StreamInterface.h"
#   include "Interface/MemoryInterface.h"

#   include "Core/ConstString.h"
#	include "Factory/FactorablePtr.h"

#   include "Config/Blobject.h"

#	include "Core/Magic.h"

namespace Menge
{
	DECLARE_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA, 'A', 'C', 'D', '1', 1 );

    class AccountInterface
		: public FactorablePtr
    {
    public:
        virtual const WString & getName() const = 0;
        virtual const ConstString & getFolder() const = 0;

    public:
		virtual bool addSetting( const ConstString & _setting, const WString& _defaultValue, const pybind::object & _applyFunc ) = 0;
        virtual bool changeSetting( const ConstString & _setting, const WString& _value ) = 0;
        virtual const WString& getSetting( const ConstString & _setting ) const = 0;
        virtual bool hasSetting( const ConstString & _setting ) const = 0;

    public:
        virtual void apply() = 0;

        virtual bool load() = 0;
        virtual bool save() = 0;

	public:
		virtual InputStreamInterfacePtr openReadBinaryFile( const FilePath & _fileName ) = 0;
		virtual OutputStreamInterfacePtr openWriteBinaryFile( const FilePath & _fileName ) = 0;

    public:		
        virtual CacheBufferID loadBinaryFile( const ConstString & _filename, const void ** _data, size_t & _size ) = 0;        
		virtual bool writeBinaryFile( const ConstString & _filename, const void * _data, size_t _size ) = 0;
		virtual bool hasBinaryFile( const ConstString & _filename ) const = 0;
    };

	typedef stdex::intrusive_ptr<AccountInterface> AccountInterfacePtr;

    class AccountProviderInterface
    {
	public:
		AccountProviderInterface(){};
		virtual ~AccountProviderInterface(){};

    public:
        virtual void onCreateAccount( const WString & _accountID ) = 0;
        virtual void onDeleteAccount( const WString & _accountID ) = 0;
        virtual void onSelectAccount( const WString & _accountID ) = 0;
        virtual void onUnselectAccount( const WString & _accountID ) = 0;
    };

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
        virtual bool initialize( const FilePath & _accountsPath, uint32_t _projectVersion, AccountProviderInterface * _listener ) = 0;
        virtual void finalize() = 0;

    public:
        virtual AccountInterfacePtr createAccount() = 0;

    public:
        virtual void deleteAccount( const WString& _accountID ) = 0;
        virtual bool selectAccount( const WString& _accountID ) = 0;

    public:
        virtual bool loadAccounts() = 0;
        virtual bool saveAccounts() = 0;

    public:
        virtual void setDefaultAccount( const WString & _accountID ) = 0;
        virtual const WString & getDefaultAccount() const = 0;
		virtual bool isCurrentDefaultAccount() const = 0;

        virtual bool hasDefaultAccount() const = 0;

        virtual bool selectDefaultAccount() = 0;

    public:
        virtual bool hasCurrentAccount() const = 0;
        virtual const AccountInterfacePtr & getCurrentAccount() = 0;
        virtual AccountInterfacePtr getAccount( const WString& _accountID ) = 0;

        virtual void visitAccounts( AccountVisitorInterface * _visitor ) const = 0;
    };

#   define ACCOUNT_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::AccountServiceInterface)
}