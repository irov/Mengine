#   pragma once

#   include "Interface/PackageInterface.h"
#   include "Interface/NotificationServiceInterface.h"

#	include "Package.h"

#	include "Factory/FactoryStore.h"

namespace Menge
{
    class PackageService
		: public ServiceBase<PackageServiceInterface>
    {
    public:
		PackageService();
		~PackageService();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool loadPackages( const ConstString & _fileGroup, const FilePath & _resourceIni ) override;

	public:
		bool addPackage( const PackageDesc & _desc ) override;
		PackageInterfacePtr getPackage( const ConstString & _name ) const override;

	public:
		bool enablePackages( const ConstString & _locale, const ConstString & _platformName ) override;
		bool validatePackages() const override;

	public:
		bool enableLocalePackage( const ConstString & _locale, const ConstString & _platformName );
		bool disableLocalePackage( const ConstString & _locale, const ConstString & _platformName );

	protected:
		void notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale );

	protected:
		ObserverInterfacePtr m_observerChangeLocale;

	protected:
		typedef stdex::vector<PackagePtr> TVectorPackage;
		TVectorPackage m_packages;

		typedef FactoryPoolStore<Package, 8> TFactoryPackage;
		TFactoryPackage m_factoryPackage;

	protected:
		bool loadLocalePacksByName_( TVectorPackage & _packs, const ConstString & _locale, const ConstString & _platform ) const;
    };
}