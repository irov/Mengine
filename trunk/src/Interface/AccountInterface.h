#   pragma once

#   include "Interface/ServiceInterface.h"
#   include "Interface/ScriptSystemInterface.h"

#   include "Core/ConstString.h"

namespace Menge
{
    class AccountInterface
    {
    public:
        virtual const WString & getName() const = 0;
        virtual const FilePath & getFolder() const = 0;

    public:
        virtual void addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc ) = 0;
        virtual void changeSetting( const WString& _setting, const WString& _value ) = 0;
        virtual const WString& getSetting( const WString& _setting ) const = 0;
        virtual bool hasSetting( const WString& _setting ) const = 0;

    public:
        virtual void apply() = 0;

        virtual bool load() = 0;
        virtual bool save() = 0;

    public:
        virtual bool loadBinaryFile( const FilePath & _filename, TBlobject & _data ) = 0;
        virtual bool writeBinaryFile( const FilePath & _filename, const TBlobject & _data ) = 0;
    };

    class AccountServiceListener
    {
    public:
        virtual void onCreateAccount( const WString & _accountID ) = 0;
        virtual void onDeleteAccount( const WString & _accountID ) = 0;
        virtual void onSelectAccount( const WString & _accountID ) = 0;
        virtual void onUnselectAccount( const WString & _accountID ) = 0;
    };

    typedef std::map<WString, AccountInterface *> TMapAccounts;

    class AccountServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("AccountService")

    public:
        virtual bool initialize( const FilePath & _accountsPath, AccountServiceListener * _listener ) = 0;
        virtual void finalize() = 0;

    public:
        virtual AccountInterface * createAccount() = 0;

    public:
        virtual void addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc ) = 0;
        virtual void changeSetting( const WString & _setting, const WString & _value ) = 0;

    public:
        virtual void deleteAccount( const WString& _accountID ) = 0;
        virtual bool selectAccount( const WString& _accountID ) = 0;
        virtual bool saveAccount( const WString& _accountID ) = 0;

    public:
        virtual void saveAccounts() = 0;

    public:
        virtual bool loadAccountsInfo() = 0;
        virtual bool saveAccountsInfo() = 0;

    public:
        virtual void setDefaultAccount( const WString & _accountID ) = 0;
        virtual const WString & getDefaultAccount() const = 0;

        virtual bool hasDefaultAccount() const = 0;

        virtual bool selectDefaultAccount() = 0;

    public:
        virtual bool hasCurrentAccount() const = 0;
        virtual AccountInterface * getCurrentAccount() = 0;
        virtual AccountInterface * getAccount( const WString& _accountID ) = 0;

        virtual const TMapAccounts & getAccounts() const = 0;
    };

#   define ACCOUNT_SERVICE( serviceProvider )\
    (Menge::getService<Menge::AccountServiceInterface>(serviceProvider))
}