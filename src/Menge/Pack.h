#	pragma once

#   include "Interface/GameInterface.h"
#   include "Interface/ResourceInterface.h"
#   include "Interface/ScriptSystemInterface.h"

#	include "Core/String.h"
#   include "Core/FilePath.h"

#	include "Factory/FactorablePtr.h"

namespace Menge
{
	class Pack
		: public PackInterface
	{
	public:
		Pack();

	public:
		void setup( ServiceProviderInterface * _serviceProvider
			, const ConstString & _name
			, const ConstString & _type
			, const ConstString & _locale
			, const ConstString & _platform
			, const ConstString & _descriptionPath
			, const ConstString & _path
			, bool _preload
			);

	public:
		const ConstString & getName() const override;

	public:
		void setPreload( bool _value ) override;
		bool isPreload() const override;

		void setLocale( const ConstString & _locale ) override;
		const ConstString & getLocale() const override;

		void setPlatfrom( const ConstString & _platform ) override;
		const ConstString & getPlatfrom() const override;

		void setPath( const ConstString & _path ) override;
		const ConstString & getPath() const override;

	public:
		bool load() override;
		bool apply() override;
		
	protected:
		bool mountFileGroup_();
		bool loadPak_();

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addResource_( const ConstString & _path );
        void addTextPath_( const ConstString & _path );
		void addScriptPak_( const ConstString & _path, const ConstString & _module, const ConstString & _initializer );
		void addFontPath_( const ConstString & _font );
		void addData_( const ConstString & _name, const ConstString & _path );
		void addMaterial_( const ConstString & _path );

    protected:
        bool loadText_( const ConstString & _pakName, const ConstString & _path );
		bool loadFont_( const ConstString & _pakName, const ConstString & _path );
		bool loadData_( const ConstString & _pakName, const ConstString & _name, const FilePath & _path );
		bool loadMaterials_( const ConstString & _pakName, const FilePath & _path );
				
	protected:
        ServiceProviderInterface * m_serviceProvider;

		TVectorConstString m_resourcesDesc;
				
		ConstString m_name;
		ConstString m_type;
		ConstString m_locale;

		ConstString m_platform;		
		FilePath m_descriptionPath;

		ConstString m_path;

		TVectorScriptModulePak m_scriptsPak;

		TVectorConstString m_pathFonts;
		TVectorConstString m_pathTexts;

		struct PakDataDesc
		{
			ConstString name;
			FilePath path;
		};

		typedef stdex::vector<PakDataDesc> TVectorPakDataDesc;
		TVectorPakDataDesc m_datas;

		TVectorConstString m_materials;

        bool m_preload;
	};

	typedef stdex::intrusive_ptr<Pack> PackPtr;
}