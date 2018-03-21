#   pragma once

#include "Interface/PackageInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Core/ServiceBase.h"

#include "Package.h"

#include "Factory/Factory.h"

namespace Mengine
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
		bool hasPackage( const ConstString & _name ) const override;
		bool addPackage( const PackageDesc & _desc ) override;
		bool removePackage( const ConstString & _name ) override;
		PackageInterfacePtr getPackage( const ConstString & _name ) const override;

	public:
		bool enablePackages( const ConstString & _locale, const Tags & _platformTags ) override;
		bool validatePackages() const override;

	public:
		bool enableLocalePackage( const ConstString & _locale, const Tags & _platformTag );
		bool disableLocalePackage( const ConstString & _locale, const Tags & _platformTag );

	protected:
		void notifyChangeLocale( const ConstString & _prevLocale, const ConstString & _currentlocale );

	protected:
		ObserverInterfacePtr m_observerChangeLocale;

	protected:
		typedef stdex::vector<PackagePtr> TVectorPackage;
		TVectorPackage m_packages;

		FactoryPtr m_factoryPackage;

	protected:
		bool loadLocalePacksByName_( TVectorPackage & _packs, const ConstString & _locale, const Tags & _platformTags ) const;
    };
}