#   pragma once

#	include "Interface/ServiceInterface.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

#	include "Factory/FactorablePtr.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct PackageDesc
	{
		ConstString name;
		ConstString type;

		ConstString locale;
		ConstString platform;

		ConstString path;
		ConstString descriptionPath;

		bool dev;
		bool immediately;
		bool preload;
	};
	//////////////////////////////////////////////////////////////////////////
	class PackageInterface
		: public FactorablePtr
	{
	public:
		virtual const ConstString & getName() const = 0;

	public:
		virtual void setPreload( bool _value ) = 0;
		virtual bool isPreload() const = 0;

		virtual void setLocale( const ConstString & _locale ) = 0;
		virtual const ConstString & getLocale() const = 0;

		virtual void setPlatfrom( const ConstString & _platform ) = 0;
		virtual const ConstString & getPlatfrom() const = 0;

		virtual void setPath( const ConstString & _path ) = 0;
		virtual const ConstString & getPath() const = 0;

	public:
		virtual bool load() = 0;
		virtual bool enable() = 0;
		virtual bool disable() = 0;

	public:
		virtual bool validate() = 0;

	public:
		virtual bool isLoad() const = 0;
		virtual bool isEnable() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<PackageInterface> PackageInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class PackageServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "PackageService" );

	public:
		virtual bool loadPackages( const ConstString & _fileGroup, const FilePath & _resourceIni ) = 0;

	public:
		virtual bool addPackage( const PackageDesc & _desc ) = 0;
		virtual PackageInterfacePtr getPackage( const ConstString & _name ) const = 0;

	public:
		virtual bool enablePackages( const ConstString & _locale, const ConstString & _platformName ) = 0;

	public:
		virtual bool validatePackages() const = 0;
	};

#   define PACKAGE_SERVICE( serviceProvider )\
    ((Menge::PackageServiceInterface*)SERVICE_GET(serviceProvider, Menge::PackageServiceInterface))
}