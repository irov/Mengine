#	pragma once

#   include "Interface/ResourceInterface.h"

#	include "Core/String.h"
#   include "Core/FilePath.h"

#	include "Factory/FactorablePtr.h"

namespace Menge
{
	class Pak
		: public FactorablePtr
	{
	public:
		Pak();

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
		bool isPreload() const;

		const ConstString & getName() const;
		const ConstString & getLocale() const;
		const ConstString & getPlatfrom() const;

		const ConstString & getPath() const;

	public:
		bool load();
		bool apply();
		
	protected:
		bool mountFileGroup_();
		bool loadPak_();

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addResource_( const ConstString & _path );
        void addTextPath_( const ConstString & _path );
		void addScriptPath_( const ConstString & _path );
		void addScriptModule_( const ConstString & _path );
		void addFontPath_( const ConstString & _font );

    protected:
        bool loadText_( const ConstString & _pakName, const ConstString & _path );
		bool loadFont_( const ConstString & _pakName, const ConstString & _path );
				
	protected:
        ServiceProviderInterface * m_serviceProvider;

		TVectorConstString m_resourcesDesc;
				
		ConstString m_name;
		ConstString m_type;
		ConstString m_locale;

		ConstString m_platform;		
		FilePath m_descriptionPath;

		ConstString m_path;

		TVectorConstString m_pathScripts;
		TVectorConstString m_pathModules;

		TVectorConstString m_pathFonts;
		TVectorConstString m_pathTexts;

        bool m_preload;
	};

	typedef stdex::intrusive_ptr<Pak> PakPtr;
}