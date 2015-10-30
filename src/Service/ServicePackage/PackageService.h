#   pragma once

#   include "Interface/PackageInterface.h"

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
		bool applyPackages() override;

	protected:
		typedef stdex::vector<PackagePtr> TVectorPackage;
		TVectorPackage m_packages;

		typedef FactoryPoolStore<Package, 8> TFactoryPackage;
		TFactoryPackage m_factoryPackage;

	protected:
		bool loadLocalePacksByName_( TVectorPackage & _packs, const ConstString & _locale, const ConstString & _platform ) const;
    };
}