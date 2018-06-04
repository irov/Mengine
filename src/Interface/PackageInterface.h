#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Core/ConstString.h"
#include "Core/Tags.h"
#include "Core/FilePath.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	struct PackageDesc
	{
		ConstString name;
		ConstString type;

		ConstString locale;
		Tags platform;

        ConstString category;
		FilePath descriptionPath;
		FilePath path;

		bool dev;
		bool immediately;
		bool preload;
	};
	//////////////////////////////////////////////////////////////////////////
	class PackageInterface
		: public ServantInterface
	{
	public:
		virtual const ConstString & getName() const = 0;

	public:
		virtual void setPreload( bool _value ) = 0;
		virtual bool isPreload() const = 0;

		virtual void setLocale( const ConstString & _locale ) = 0;
		virtual const ConstString & getLocale() const = 0;

		virtual void setPlatfromTags( const Tags & _platform ) = 0;
		virtual const Tags & getPlatfromTags() const = 0;

		virtual void setPath( const FilePath & _path ) = 0;
		virtual const FilePath & getPath() const = 0;

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
	typedef IntrusivePtr<PackageInterface> PackageInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class PackageServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "PackageService" );

	public:
		virtual bool loadPackages( const FileGroupInterfacePtr & _fileGroup, const FilePath & _resourceIni ) = 0;

	public:
		virtual bool hasPackage( const ConstString & _name ) const = 0;
		virtual bool addPackage( const PackageDesc & _desc ) = 0;
		virtual bool removePackage( const ConstString & _name ) = 0;
		virtual PackageInterfacePtr getPackage( const ConstString & _name ) const = 0;

	public:
		virtual bool enablePackages( const ConstString & _locale, const Tags & _platformTags ) = 0;

	public:
		virtual bool validatePackages() const = 0;
	};

#   define PACKAGE_SERVICE()\
    ((Mengine::PackageServiceInterface*)SERVICE_GET(Mengine::PackageServiceInterface))
}