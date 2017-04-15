#	pragma once

#   include "Interface/PackageInterface.h"
#   include "Interface/ScriptSystemInterface.h"

#   include "Core/ServantBase.h"
#	include "Core/String.h"
#   include "Core/FilePath.h"

namespace Menge
{
	class Package
		: public ServantBase<PackageInterface>
	{
	public:
		Package();

	public:
		void setup( const ConstString & _name
			, const ConstString & _type
			, const ConstString & _locale
			, const Tags & _platform
			, const FilePath & _descriptionPath
			, const FilePath & _path
			, bool _preload
			);

	public:
		const ConstString & getName() const override;

	public:
		void setPreload( bool _value ) override;
		bool isPreload() const override;

		void setLocale( const ConstString & _locale ) override;
		const ConstString & getLocale() const override;

		void setPlatfromTags( const Tags & _tags ) override;
		const Tags & getPlatfromTags() const override;

		void setPath( const FilePath & _path ) override;
		const FilePath & getPath() const override;

	public:
		bool load() override;
		bool enable() override;
		bool disable() override;

	public:
		bool validate() override;

	public:
		bool isLoad() const override;
		bool isEnable() const override;
		
	protected:
		bool mountFileGroup_();
		bool loadPak_();

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addResource_( const FilePath & _path, const Tags & _platform, bool _ignored );
		void addTextPath_( const FilePath & _path, const Tags & _platform );
		void addScriptPak_( const FilePath & _path, const ConstString & _module, const ConstString & _initializer, const ConstString & _finalizer, const Tags & _platform );
		void addFontPath_( const FilePath & _path, const Tags & _tags );
		void addData_( const ConstString & _name, const FilePath & _path, const Tags & _platform );
		void addMaterial_( const FilePath & _path, const Tags & _platform );

    protected:
		bool loadText_( const ConstString & _pakName, const FilePath & _path );
		bool unloadText_( const ConstString & _pakName, const FilePath & _path );
		bool loadFont_( const ConstString & _pakName, const FilePath & _path );
		bool unloadFont_( const ConstString & _pakName, const FilePath & _path );
		bool addUserData_( const ConstString & _pakName, const ConstString & _name, const FilePath & _path );
		bool removeUserData_( const ConstString & _name );
		bool loadMaterials_( const ConstString & _pakName, const FilePath & _path );
		bool unloadMaterials_( const ConstString & _pakName, const FilePath & _path );
				
	protected:
		struct PakResourceDesc
		{			
			FilePath path;
			Tags platform;
			bool ignored;
		};

		typedef stdex::vector<PakResourceDesc> TVectorPakResourceDesc;
		TVectorPakResourceDesc m_resourcesDesc;
				
		ConstString m_name;
		ConstString m_type;
		ConstString m_locale;

		Tags m_platform;
		FilePath m_descriptionPath;

		FilePath m_path;

		TVectorScriptModulePack m_scriptsPackages;

		struct PakFontDesc
		{
			FilePath path;
			Tags platform;
		};

		typedef std::vector<PakFontDesc> TVectorPakFontDesc;
		TVectorPakFontDesc m_pathFonts;

		struct PakTextDesc
		{
			FilePath path;
			Tags platform;
		};

		typedef std::vector<PakTextDesc> TVectorPakTextDesc;
		TVectorPakTextDesc m_pathTexts;

		struct PakDataDesc
		{
			ConstString name;
			FilePath path;
			Tags platform;
		};

		typedef stdex::vector<PakDataDesc> TVectorPakDataDesc;
		TVectorPakDataDesc m_datas;

		struct PakMaterialDesc
		{			
			FilePath path;
			Tags platform;
		};

		typedef stdex::vector<PakMaterialDesc> TVectorPakMaterialDesc;
		TVectorPakMaterialDesc m_pathMaterials;

        bool m_preload;
		bool m_load;
		bool m_enable;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Package> PackagePtr;
}